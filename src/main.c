#include "xram.h"
#include <rp6502.h>
#include <stdio.h>

#include "input_actions.h"

int main(void)
{
    xreg_vga_canvas(CANVAS_320X240);

    // Tilemap
    mode2_config_t tilemap_config = {
        .x_wrap = false,
        .y_wrap = false,
        .x_pos_px = 0,
        .y_pos_px = 0,
        .width_tiles = WIDTH_IN_TILES,
        .height_tiles = HEIGHT_IN_TILES,
        .xram_data_ptr = XRAM_TILEMAP_DATA,
        .xram_palette_ptr = 0xffff,
        .xram_tile_ptr = XRAM_TILES,
    };

    xram0_set(XRAM_TILEMAP_DATA, 0, sizeof(uint8_t) * TILEMAP_SIZE_IN_TILES);
    xram0_set(XRAM_TILES, 0, sizeof(tile_t) * MAX_TILES);

    xram0_write(XRAM_TILEMAP_CONFIG, &tilemap_config, sizeof(tilemap_config));

    xreg_vga_mode2(MODE2_8BPP | MODE2_16X16, XRAM_TILEMAP_CONFIG, 2);

#ifdef CURSOR_AS_SPRITE
    // Sprites
    mode5_sprite_t sprite_config = {
        .x_pos_px = 20,
        .y_pos_px = 20,
        .xram_sprite_ptr = XRAM_SPRITES + 0 * sizeof(sprite_2bpp_t),
        .palette_ptr = 0xffff,
    };
    xram0_write(XRAM_SPRITE_CONFIG + 0 * sizeof(mode5_sprite_t), &sprite_config, sizeof(mode5_sprite_t));

    xreg_vga_mode5(MODE5_2BPP | MODE5_16X16, XRAM_SPRITE_CONFIG, MAX_SPRITES, 0);
#endif

    //
    puts("Hello, world!");

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
