#include "display.h"
#include "xram.h"
#include <rp6502.h>
#include <stdio.h>

#include "input_actions.h"

int main(void)
{
    puts("Program starting");

    display_init();
    input_action_init();

    while (true)
    {
        // Vsync
        const unsigned char frame = ria_vsync();
        while (ria_vsync() == frame)
            ;

        //
        const actions_t *actions = input_action_update();

        if (actions->pointer_moved)
        {
            printf("x=%d y=%d\n", actions->pointer_x, actions->pointer_y);

#ifdef CURSOR_AS_SPRITE
            sprite_config.x_pos_px = actions->pointer_x;
            sprite_config.y_pos_px = actions->pointer_y;
            xram0_write(XRAM_SPRITE_CONFIG + 0 * sizeof(mode5_sprite_t), &sprite_config, sizeof(mode5_sprite_t));
#endif
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
