#include "board.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

static uint8_t compute_close_mine_count(uint8_t x, uint8_t y)
{
    uint8_t mine_count = 0;
    for (int8_t dy = -1; dy <= 1; dy++)
    {
        for (int8_t dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
            {
                continue;
            }

            const int8_t xdx = x + dx;
            const int8_t ydy = y + dy;

            if (xdx < 0 || xdx >= BOARD_WIDTH || ydy < 0 || ydy >= BOARD_HEIGHT)
            {
                continue;
            }

            if (board_content[XY_TO_INDEX(xdx, ydy)] == CELL_MINE_MARKER)
            {
                mine_count += 1;
            }
        }
    }
    return mine_count;
}

static void compute_hints()
{
    const uint8_t h = get_board_height();
    const uint8_t w = get_board_width();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (uint8_t *cell = &board_content[XY_TO_INDEX(x, y)]; *cell != CELL_MINE_MARKER)
            {
                *cell = compute_close_mine_count(x, y);
            }
        }
    }
}

static void distribute_mines(uint8_t x, uint8_t y)
{
    uint8_t mine_count = MINE_COUNT;

    srand(time(NULL));

    while (mine_count > 0)
    {
        const uint8_t mine_x = rand() % BOARD_WIDTH;  // NOLINT(*-msc50-cpp)
        const uint8_t mine_y = rand() % BOARD_HEIGHT; // NOLINT(*-msc50-cpp)

        if (abs(mine_x - x) <= 1 && abs(mine_y - y) <= 1)
        {
            continue;
        }

        const uint8_t index = XY_TO_INDEX(mine_x, mine_y);

        if (board_content[index] == CELL_MINE_MARKER)
        {
            continue;
        }

        board_content[index] = CELL_MINE_MARKER;
        mine_count -= 1;
    }
    board_distributed = true;
}

void board_full_reveal()
{
    const uint8_t h = get_board_height();
    const uint8_t w = get_board_width();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (ECell_t *status = &board_status[XY_TO_INDEX(x, y)]; *status == ECell_Hidden)
            {
                *status = ECell_Uncovered;
            }
        }
    }
}

#define MAX_REVEAL_QUEUE 15
static uint8_t reveal_queue_x[MAX_REVEAL_QUEUE];
static uint8_t reveal_queue_y[MAX_REVEAL_QUEUE];
static uint8_t reveal_queue_index;
static uint8_t max_queue_index;

static bool queue_has(uint8_t x, uint8_t y)
{
    for (uint8_t i = 0; i < reveal_queue_index; i++)
    {
        if (reveal_queue_x[i] == x && reveal_queue_y[i] == y)
        {
            return true;
        }
    }
    return false;
}

#define max(x, y) (x > y ? x : y)

static void add_to_queue(uint8_t x, uint8_t y)
{
    if (queue_has(x, y) || reveal_queue_index == MAX_REVEAL_QUEUE)
    {
        return;
    }
    reveal_queue_x[reveal_queue_index] = x;
    reveal_queue_y[reveal_queue_index] = y;
    reveal_queue_index += 1;

    max_queue_index = max(max_queue_index, reveal_queue_index);
}

static uint8_t front_x()
{
    return reveal_queue_x[reveal_queue_index - 1];
}

static uint8_t front_y()
{
    return reveal_queue_y[reveal_queue_index - 1];
}

static void pop_queue()
{
    reveal_queue_index -= 1;
}

static bool has_queue_element()
{
    return reveal_queue_index > 0;
}

static void board_step_partial_reveal(uint8_t x, uint8_t y)
{
    for (int8_t dy = -1; dy <= 1; dy++)
    {
        for (int8_t dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
            {
                continue;
            }
            const int8_t xdx = x + dx;
            const int8_t ydy = y + dy;
            if (xdx < 0 || xdx >= BOARD_WIDTH || ydy < 0 || ydy >= BOARD_HEIGHT)
            {
                continue;
            }

            const uint8_t index = XY_TO_INDEX(xdx, ydy);

            if (board_status[index] == ECell_Hidden && board_content[index] != CELL_MINE_MARKER)
            {
                board_status[index] = ECell_Uncovered;
                if (board_content[index] == 0)
                {
                    add_to_queue(xdx, ydy);
                }
            }
        }
    }
}

void board_partial_reveal(uint8_t x, uint8_t y)
{
    if (board_content[XY_TO_INDEX(x, y)] != 0)
    {
        return;
    }
    reveal_queue_index = 0;
    max_queue_index = 0;
    add_to_queue(x, y);
    while (has_queue_element())
    {
        const uint8_t fx = front_x();
        const uint8_t fy = front_y();
        pop_queue();

        board_step_partial_reveal(fx, fy);
    }
    printf("Max queue: %u\n", max_queue_index);
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
        compute_hints();
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
