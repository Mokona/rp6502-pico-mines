#ifndef MY_RP6502_PROJECT_INPUT_ACTIONS_H
#define MY_RP6502_PROJECT_INPUT_ACTIONS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t pointer_x;
    uint16_t pointer_y;
    bool pointer_moved;
    bool just_pressed_up;
    bool just_pressed_down;
    bool just_pressed_left;
    bool just_pressed_right;
    bool just_pressed_action;
    bool just_pressed_alternate;
    bool just_pressed_start;
    bool just_pressed_select;
    uint8_t dpad;
    uint8_t tablet_status;
} actions_t;

void input_action_init();
const actions_t * input_action_update();

#endif // MY_RP6502_PROJECT_INPUT_ACTIONS_H
