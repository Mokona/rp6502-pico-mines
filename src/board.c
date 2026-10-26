#include "board.h"

// Hard coded at the moment
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

void board_init()
{
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
    return 0;
}

ECell_t get_cell_type(uint8_t x, uint8_t y)
{
    return ECell_Hidden;
}
