#include "board.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Hard coded at the moment
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10
#define MINE_COUNT 10

static bool board_distributed;
static uint8_t board_content[BOARD_WIDTH * BOARD_HEIGHT];
static ECell_t board_status[BOARD_WIDTH * BOARD_HEIGHT];

#define XY_TO_INDEX(x, y) ((x) + (y) * BOARD_WIDTH)

void board_init()
{
    memset(board_content, 0, sizeof(board_content));
    memset(board_status, ECell_Hidden, sizeof(board_status));
    board_distributed = false;
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

static void distribute_mines(uint8_t x, uint8_t y)
{
    uint8_t mine_count = MINE_COUNT;

    while (mine_count > 0)
    {
        uint8_t mine_x = rand() % BOARD_WIDTH;  // NOLINT(*-msc50-cpp)
        uint8_t mine_y = rand() % BOARD_HEIGHT; // NOLINT(*-msc50-cpp)

        if (abs(mine_x - x) <= 1 && abs(mine_y - y) <= 1)
        {
            continue;
        }

        board_content[XY_TO_INDEX(mine_x, mine_y)] = CELL_MINE_MARKER;
        mine_count -= 1;
    }
    board_distributed = true;
}

void press_on_cell(uint8_t x, uint8_t y)
{
    if (x >= BOARD_WIDTH || y >= BOARD_HEIGHT)
    {
        return;
    }

    if (!board_distributed)
    {
        distribute_mines(x, y);
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
