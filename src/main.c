#include "display.h"
#include "xram.h"
#include <rp6502.h>
#include <stdio.h>

#include "board.h"
#include "input_actions.h"
#include "game_config.h"
#include "ui.h"

static void draw_board_borders()
{
    const uint8_t h = get_board_height();
    const uint8_t w = get_board_width();

    for (int x = 0; x < w; x++)
    {
        display_tile_at(BOARD_DISPLAY_X + x, BOARD_DISPLAY_Y - 1, BORDER_U);
        display_tile_at(BOARD_DISPLAY_X + x, BOARD_DISPLAY_Y + h, BORDER_D);
    }
    for (int y = 0; y < h; y++)
    {
        display_tile_at(BOARD_DISPLAY_X - 1, BOARD_DISPLAY_Y + y, BORDER_L);
        display_tile_at(BOARD_DISPLAY_X + w, BOARD_DISPLAY_Y + y, BORDER_R);
    }

    display_tile_at(BOARD_DISPLAY_X - 1, BOARD_DISPLAY_Y - 1, BORDER_UL);
    display_tile_at(BOARD_DISPLAY_X + w, BOARD_DISPLAY_Y - 1, BORDER_UR);
    display_tile_at(BOARD_DISPLAY_X + w, BOARD_DISPLAY_Y + h, BORDER_DR);
    display_tile_at(BOARD_DISPLAY_X - 1, BOARD_DISPLAY_Y + h, BORDER_DL);
}

static void draw_board()
{
    const uint8_t h = get_board_height();
    const uint8_t w = get_board_width();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            ECell_t cell = get_cell_type(x, y);
            uint8_t tile_index = UI_SAD; // in case of program error, a sad face will be displayed
            switch (cell)
            {
            case ECell_Hidden:
                tile_index = CELL_HIDDEN;
                break;
            case ECell_Flag:
                tile_index = CELL_FLAG;
                break;
            case ECell_Uncovered:
                tile_index = get_cell_content(x, y);
                if (tile_index == CELL_MINE_MARKER)
                {
                    tile_index = CELL_MINE;
                }
                else
                {
                    tile_index += HINT_1 - 1;
                }
                break;
            case ECell_Exploded:
                tile_index = CELL_EXPLODE;
                break;
            default:
                break;
            }

            display_tile_at(BOARD_DISPLAY_X + x, BOARD_DISPLAY_Y + y, tile_index);
        }
    }
}

int main(void)
{
    puts("Program starting");

    display_init();
    input_action_init();
    board_init();

    draw_board();
    draw_board_borders();
    draw_ui();

    uint8_t old_hover_x = 0;
    uint8_t old_hover_y = 0;

    while (true)
    {
        // Vsync
        const unsigned char frame = ria_vsync();
        while (ria_vsync() == frame)
            ;

        //
        const actions_t *actions = input_action_update();

        if (actions->pointer_moved)
        {
            const int8_t tentative_hover_cell_x = (int8_t)(actions->pointer_x / 16 - BOARD_DISPLAY_X);
            const int8_t tentative_hover_cell_y = (int8_t)(actions->pointer_y / 16 - BOARD_DISPLAY_Y);

            if ((tentative_hover_cell_x >= 0 && tentative_hover_cell_x < get_board_width()) &&
                (tentative_hover_cell_y >= 0 && tentative_hover_cell_y < get_board_height()))
            {
                const uint8_t hover_cell_x = tentative_hover_cell_x;
                const uint8_t hover_cell_y = tentative_hover_cell_y;

                if (hover_cell_x != old_hover_x || hover_cell_y != old_hover_y)
                {
                    // printf("x=%d y=%d\n", hover_cell_x, hover_cell_y);

                    old_hover_x = hover_cell_x;
                    old_hover_y = hover_cell_y;
                }
            }
            else
            {
                old_hover_x = 0xff;
                old_hover_y = 0xff;
            }

#ifdef CURSOR_AS_SPRITE
            sprite_config.x_pos_px = actions->pointer_x;
            sprite_config.y_pos_px = actions->pointer_y;
            xram0_write(XRAM_SPRITE_CONFIG + 0 * sizeof(mode5_sprite_t), &sprite_config, sizeof(mode5_sprite_t));
#endif
        }

        if (actions->just_pressed_left)
        {
            puts("DPAD_LEFT");
        }

        if (actions->just_pressed_action)
        {
            press_on_cell(old_hover_x, old_hover_y);
            draw_board(); // TODO: optimize by displaying only the changed cell

            const ECell_t cell = get_cell_type(old_hover_x, old_hover_y);
            if (cell == ECell_Exploded)
            {
                // Game over
                board_full_reveal();
                draw_board();
                draw_sad_face();
            }
            else
            {
                board_partial_reveal(old_hover_x, old_hover_y);
                draw_board();
            }

            if (has_won())
            {
                draw_happy_face();
            }
        }
        else if (actions->just_pressed_alternate)
        {
            flag_on_cell(old_hover_x, old_hover_y);
            draw_board(); // TODO: optimize by displaying only the changed cell
            draw_flags(MINE_COUNT - get_flag_count());
        }
    }
}
