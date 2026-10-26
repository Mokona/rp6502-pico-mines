#include "display.h"
#include "xram.h"
#include <rp6502.h>
#include <stdio.h>

#include "board.h"
#include "input_actions.h"

#define BOARD_DISPLAY_X 5
#define BOARD_DISPLAY_Y 3

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
            printf("x=%d y=%d\n", actions->pointer_x, actions->pointer_y);

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
            puts("ACTION");
        }
    }
}
