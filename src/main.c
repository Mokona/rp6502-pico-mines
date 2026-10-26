#include "xram.h"
#include <rp6502.h>
#include <stdio.h>

#include "input_actions.h"

int main(void)
{
    puts("Hello, world!");

    input_action_init();

    while (true)
    {
        const actions_t *actions = input_action_update();

        if (actions->pointer_moved)
        {
            printf("x=%d y=%d\n", actions->pointer_x, actions->pointer_y);
        }

        if (actions->just_pressed_left)
        {
            puts("DPAD_LEFT");
        }

        if (actions->just_pressed_action)
        {
            puts("ACTION");
        }
    }
}
