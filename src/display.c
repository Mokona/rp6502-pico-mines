#include "display.h"

#include "xram.h"

void display_init()
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

    // Create the black tile
    xram0_set(XRAM_TILES + MAX_TILES * sizeof(tile_t), 0, sizeof(tile_t));
    xram0_set(XRAM_TILEMAP_DATA, MAX_TILES, TILEMAP_SIZE_IN_TILES);

    xram0_write(XRAM_TILEMAP_CONFIG, &tilemap_config, sizeof(tilemap_config));
    xreg_vga_mode2(MODE2_8BPP | MODE2_16X16, XRAM_TILEMAP_CONFIG, 2);

    for (int i = 0; i < MAX_TILES; ++i)
    {
        xram0_poke8(XRAM_TILEMAP_DATA + i, i);
    }

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

}

void display_tile_at(uint8_t x, uint8_t y, uint8_t tile)
{
    xram0_poke8(XRAM_TILEMAP_DATA + x + y * WIDTH_IN_TILES, tile);
}
