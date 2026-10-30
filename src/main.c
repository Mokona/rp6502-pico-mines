#include <rp6502.h>
#include <stdio.h>

#include "board.h"
#include "display.h"
#include "game_config.h"
#include "input_actions.h"
#include "ui.h"
#include "xram.h"

#define BLINK_TIME 20

static bool hover_blink_on;
static uint8_t hover_blink_time;

static uint8_t time_counter;
static int16_t game_time;

static uint8_t current_level;

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

static void draw_cell(int x, int y)
{
    const ECell_t cell = get_cell_type(x, y);
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

static void draw_board()
{
    const uint8_t h = get_board_height();
    const uint8_t w = get_board_width();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            draw_cell(x, y);
        }
    }
}

static void draw_cell_blink(uint8_t x, uint8_t y, bool blink)
{
    if (x != 0xff && y != 0xff && x < get_board_width() && y < get_board_height())
    {
        if (blink)
        {
            display_tile_at(BOARD_DISPLAY_X + x, BOARD_DISPLAY_Y + y, BG__C);
        }
        else
        {
            draw_cell(x, y);
        }
    }
}

static uint8_t old_hover_x;
static uint8_t old_hover_y;

static void main_game_update(bool *game_running, const actions_t *actions)
{
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
                draw_cell_blink(old_hover_x, old_hover_y, false);
                hover_blink_on = true;
                hover_blink_time = BLINK_TIME;

                old_hover_x = hover_cell_x;
                old_hover_y = hover_cell_y;
            }
        }
        else
        {
            draw_cell_blink(old_hover_x, old_hover_y, false);

            old_hover_x = 0xff;
            old_hover_y = 0xff;
        }

#ifdef CURSOR_AS_SPRITE
        sprite_config.x_pos_px = actions->pointer_x;
        sprite_config.y_pos_px = actions->pointer_y;
        xram0_write(XRAM_SPRITE_CONFIG + 0 * sizeof(mode5_sprite_t), &sprite_config, sizeof(mode5_sprite_t));
#endif
    }

    // TODO: simplify the duplicated code
    if (actions->just_pressed_left)
    {
        draw_cell_blink(old_hover_x, old_hover_y, false);
        hover_blink_on = true;
        hover_blink_time = BLINK_TIME;

        if (old_hover_x > 0)
        {
            old_hover_x -= 1;
        }
    }

    if (actions->just_pressed_right)
    {
        draw_cell_blink(old_hover_x, old_hover_y, false);
        hover_blink_on = true;
        hover_blink_time = BLINK_TIME;

        if (old_hover_x < get_board_width() - 1)
        {
            old_hover_x += 1;
        }
    }

    if (actions->just_pressed_up)
    {
        draw_cell_blink(old_hover_x, old_hover_y, false);
        hover_blink_on = true;
        hover_blink_time = BLINK_TIME;

        if (old_hover_y > 0)
        {
            old_hover_y -= 1;
        }
    }

    if (actions->just_pressed_down)
    {
        draw_cell_blink(old_hover_x, old_hover_y, false);
        hover_blink_on = true;
        hover_blink_time = BLINK_TIME;

        if (old_hover_y < get_board_height() - 1)
        {
            old_hover_y += 1;
        }
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
            *game_running = false;
        }
        else
        {
            board_partial_reveal(old_hover_x, old_hover_y);
            draw_board();
        }

        if (has_won())
        {
            draw_happy_face();
            *game_running = false;
        }
    }
    else if (actions->just_pressed_alternate)
    {
        flag_on_cell(old_hover_x, old_hover_y);
        draw_board(); // TODO: optimize by displaying only the changed cell

        const int8_t left_mines = MINE_COUNT - get_flag_count();
        draw_flags(max(left_mines, 0));
    }

    time_counter += 1;
    if (time_counter >= 60)
    {
        time_counter = 0;
        if (game_time < 999)
        {
            game_time += 1;
        }
        draw_time(game_time);
    }

    if (*game_running)
    {
        draw_cell_blink(old_hover_x, old_hover_y, hover_blink_on);

        hover_blink_time -= 1;
        if (hover_blink_time == 0)
        {
            hover_blink_time = BLINK_TIME;
            hover_blink_on = !hover_blink_on;
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

    hover_blink_on = false;
    hover_blink_time = BLINK_TIME;

    current_level = 1;

    old_hover_x = get_board_width() / 2;
    old_hover_y = get_board_height() / 2;

    bool game_running = false;

    while (true)
    {
        // Vsync
        const unsigned char frame = ria_vsync();
        while (ria_vsync() == frame)
            ;

        //
        const actions_t *actions = input_action_update();

        if (game_running)
        {
            main_game_update(&game_running, actions);
        }
        else
        {
            const uint16_t pointer_x = actions->pointer_x;
            const uint16_t pointer_y = actions->pointer_y;

            // const  bool setting = is_hovering_gear(actions->pointer_x, actions->pointer_y);

            if (actions->just_pressed_action && is_hovering_face(pointer_x, pointer_y) || //
                actions->just_pressed_start)
            {
                game_running = true;
            }

            if (actions->just_pressed_action && is_hovering_gear(pointer_x, pointer_y) || //
                actions->just_pressed_select)
            {
                current_level += 1;
                if (current_level > MAX_LEVELS)
                {
                    current_level = 1;
                }
                draw_level(current_level);
                draw_flags(get_mines_for_level(current_level));
            }

            if (game_running)
            {
                puts("Let's go!");
                draw_timer();
                draw_time(0);

                board_init();
                draw_board();

                time_counter = 0;
                game_time = 0;
            }
        }
    }
}
