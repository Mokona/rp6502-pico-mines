#include "ui.h"

#include "display.h"
#include "game_config.h"
#include "generated/tile_atlas.h"

#define COUNTER_FACE_X (BOARD_DISPLAY_X - 1)

static void display_number_at(uint8_t x, uint8_t y, uint16_t number, uint8_t width)
{
    x += width - 1;
    while (width > 0)
    {
        const uint8_t rest = number % 10;
        display_tile_at(x, y, NUMBER_0 + rest);
        x -= 1;
        width -= 1;
        number /= 10;
    }
}

void draw_happy_face()
{
    display_tile_at(COUNTER_FACE_X, BOARD_DISPLAY_Y - 2, UI_HAPPY);
}

void draw_sad_face()
{
    display_tile_at(COUNTER_FACE_X, BOARD_DISPLAY_Y - 2, UI_SAD);
}

void draw_ui()
{
    display_tile_at(COUNTER_FACE_X, BOARD_DISPLAY_Y - 2, UI_TIME);
    display_number_at(COUNTER_FACE_X + 1, BOARD_DISPLAY_Y - 2, 999, 3);

    display_tile_at(COUNTER_FACE_X + 6, BOARD_DISPLAY_Y - 2, UI_GEAR);
    display_number_at(COUNTER_FACE_X + 7, BOARD_DISPLAY_Y - 2, 1, 1);

    display_tile_at(COUNTER_FACE_X + 9, BOARD_DISPLAY_Y - 2, UI_FLAG);
    display_number_at(COUNTER_FACE_X + 10, BOARD_DISPLAY_Y - 2, 0, 2);
}
