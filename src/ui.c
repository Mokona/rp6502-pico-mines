#include "ui.h"

#include "display.h"
#include "game_config.h"
#include "generated/tile_atlas.h"

#include <stdio.h>

#define FACE_CELL_X (BOARD_DISPLAY_X - 1)
#define FACE_CELL_Y (BOARD_DISPLAY_Y - 2)

#define GEAR_CELL_X (FACE_CELL_X + 6)
#define GEAR_CELL_Y (FACE_CELL_Y)

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
    display_tile_at(FACE_CELL_X, FACE_CELL_Y, UI_HAPPY);
}

void draw_sad_face()
{
    display_tile_at(FACE_CELL_X, FACE_CELL_Y, UI_SAD);
}

void draw_timer()
{
    display_tile_at(FACE_CELL_X, FACE_CELL_Y, UI_TIME);
}

void draw_time(uint16_t time)
{
    display_number_at(FACE_CELL_X + 1, FACE_CELL_Y, time, 3);
}

void draw_flags(uint8_t count)
{
    display_number_at(FACE_CELL_X + 10, FACE_CELL_Y, count, 2);
}

void draw_ui()
{
    draw_happy_face();
    draw_time(999);

    display_tile_at(GEAR_CELL_X, GEAR_CELL_Y, UI_GEAR);
    display_number_at(GEAR_CELL_X + 1, GEAR_CELL_Y, 1, 1);

    display_tile_at(FACE_CELL_X + 9, FACE_CELL_Y, UI_FLAG);
    draw_flags(10);
}

bool is_hovering_gear(uint16_t x, uint16_t y)
{
    const uint8_t cell_x = (x / 16) - GEAR_CELL_X;
    const uint8_t cell_y = (y / 16) - GEAR_CELL_Y;

    return cell_x == 0 && cell_y == 0;
}

bool is_hovering_face(uint16_t x, uint16_t y)
{
    const uint8_t cell_x = (x / 16) - FACE_CELL_X;
    const uint8_t cell_y = (y / 16) - FACE_CELL_Y;

    return cell_x == 0 && cell_y == 0;
}
