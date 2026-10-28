#include "board.h"

#include <string.h>

// Hard coded at the moment
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

static uint8_t board_content[BOARD_WIDTH * BOARD_HEIGHT];
static ECell_t board_status[BOARD_WIDTH * BOARD_HEIGHT];

#define XY_TO_INDEX(x, y) ((x) + (y) * BOARD_WIDTH)

void board_init()
{
    memset(board_content, 0, sizeof(board_content));
    memset(board_status, ECell_Hidden, sizeof(board_status));
}

uint8_t get_board_width()
{
    return BOARD_WIDTH;
}

uint8_t get_board_height()
{
    return BOARD_HEIGHT;
}

uint8_t get_cell_content(uint8_t x, uint8_t y)
{
    return board_content[XY_TO_INDEX(x, y)];
}

ECell_t get_cell_type(uint8_t x, uint8_t y)
{
    return board_status[XY_TO_INDEX(x, y)];
}
void press_on_cell(uint8_t x, uint8_t y)
{
    if (x >= BOARD_WIDTH || y >= BOARD_HEIGHT)
    {
        return;
    }

    if (get_cell_type(x, y) == ECell_Hidden)
    {
        ECell_t new_status = ECell_Uncovered;
        if (get_cell_content(x, y) == CELL_MINE_MARKER)
        {
            new_status = ECell_Exploded;
        }
        board_status[XY_TO_INDEX(x, y)] = new_status;
    }
}
