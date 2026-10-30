#ifndef MY_RP6502_PROJECT_BOARD_H
#define MY_RP6502_PROJECT_BOARD_H

#include <stdint.h>
#include <stdbool.h>

void board_init();

typedef enum
{
    ECell_Hidden,
    ECell_Flag,
    ECell_Uncovered,
    ECell_Exploded,
} ECell_t;

uint8_t get_board_width();
uint8_t get_board_height();

#define CELL_MINE_MARKER 255

// 0-8 = neighbours ; 255 = (CELL_MINE_MARKER)
uint8_t get_cell_content(uint8_t x, uint8_t y);
ECell_t get_cell_type(uint8_t x, uint8_t y);

void press_on_cell(uint8_t x, uint8_t y);
void board_full_reveal();
void board_partial_reveal(uint8_t x, uint8_t y);

void flag_on_cell(uint8_t x, uint8_t y);
uint8_t get_flag_count();

bool has_won();

uint8_t get_mines_for_level(uint8_t level);


#endif // MY_RP6502_PROJECT_BOARD_H
