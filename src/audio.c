#include "audio.h"

#include "xram.h"

#include <stdio.h>

static const uint8_t attack_frames[16] = {1, 1, 1, 2, 3, 4, 5, 5, 6, 15, 30, 48, 60, 180, 255, 255};
static const uint8_t decay_frames[16] = {1, 2, 3, 5, 7, 11, 13, 15, 18, 45, 90, 144, 180, 255, 255, 255};

void audio_init()
{
    xreg_ria_psg(XRAM_PSG);
}

static uint8_t gate_timer[8];

static void psg_play(uint8_t ch, uint16_t freq, uint8_t duty, uint8_t atk, uint8_t dec, uint8_t rw, uint8_t pg)
{
    const uint16_t frames = attack_frames[atk & 0x0F] + decay_frames[dec & 0x0F];
    const psg_channel_t c = {.freq = freq, .duty = duty, .attack = atk, .decay = dec, .release_wave = rw, .pan_gate = pg, .reserved = 0};
    xram0_write(XRAM_PSG + ch * 8, &c, sizeof(c));
    gate_timer[ch] = frames;
}

void play_press()
{
    printf("Play\n");
    psg_play(0, PSG_FREQ_HZ(300), 64, 0x00, 0xF1, PSG_WAVE_SQUARE | 0x0, PSG_PAN(0) | PSG_GATE);
}

void play_put_flag()
{
}

void play_remove_flag()
{
}

void audio_update()
{
    for (uint8_t ch = 0; ch < 8; ++ch)
    {
        if (gate_timer[ch] && --gate_timer[ch] == 0)
        {
            const uint16_t a = XRAM_PSG + ch * 8 + offsetof(psg_channel_t, pan_gate);
            xram0_poke8(a, xram0_peek8(a) & ~PSG_GATE);
        }
    }
}