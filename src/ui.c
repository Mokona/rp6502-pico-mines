#include "ui.h"

#include "display.h"
#include "game_config.h"
#include "generated/tile_atlas.h"

#define COUNTER_FACE_X (BOARD_DISPLAY_X - 1)

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
    display_tile_at(COUNTER_FACE_X + 1, BOARD_DISPLAY_Y - 2, NUMBER_0);
    display_tile_at(COUNTER_FACE_X + 2, BOARD_DISPLAY_Y - 2, NUMBER_0);
    display_tile_at(COUNTER_FACE_X + 3, BOARD_DISPLAY_Y - 2, NUMBER_0);

    display_tile_at(COUNTER_FACE_X + 5, BOARD_DISPLAY_Y - 2, UI_GEAR);
    display_tile_at(COUNTER_FACE_X + 6, BOARD_DISPLAY_Y - 2, NUMBER_0);

    display_tile_at(BOARD_DISPLAY_X + 8, BOARD_DISPLAY_Y - 2, UI_FLAG);
    display_tile_at(BOARD_DISPLAY_X + 9, BOARD_DISPLAY_Y - 2, NUMBER_0);
    display_tile_at(BOARD_DISPLAY_X + 10, BOARD_DISPLAY_Y - 2, NUMBER_0);
}
