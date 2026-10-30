#include "audio.h"

#include "xram.h"

#include <stdio.h>

/* Notes */
#define REST 0
#define G3 588
#define C4 785
#define CS4 832
#define D4 881
#define DS4 933
#define E4 989
#define G4 1176
#define C5 1570
#define E5 1978
#define G5 2352
#define A5 2640
#define C6 3140

// A Note Step
typedef struct
{
    uint16_t freq;
    uint8_t frames;
} step_t;

// Instrument
typedef struct
{
    uint8_t duty;
    uint8_t attack;
    uint8_t decay;
    uint8_t release;
} instrument_t;

static const instrument_t ins_click = {.duty = 64, .attack = 0x00, .decay = 0xF1, .release = PSG_WAVE_SQUARE | 0x0};
static const instrument_t ins_flag_put = {.duty = 128, .attack = 0x00, .decay = 0xF3, .release = PSG_WAVE_SQUARE | 0x2};
static const instrument_t ins_flag_remove = {.duty = 128, .attack = 0x00, .decay = 0xF3, .release = PSG_WAVE_SQUARE | 0x2};
static const instrument_t ins_start = {64, 0x02, 0x10, PSG_WAVE_SINE | 0x4};
static const instrument_t ins_win = {64, 0x01, 0xF8, PSG_WAVE_SQUARE | 0x8};
static const instrument_t ins_boom = {128, 0x00, 0xF9, PSG_WAVE_NOISE | 0x9};
static const instrument_t ins_lose = {128, 0x13, 0xF7, PSG_WAVE_SAWTOOTH | 0x7};

static const uint8_t attack_frames[16] = {1, 1, 1, 2, 3, 4, 5, 5, 6, 15, 30, 48, 60, 180, 255, 255};
static const uint8_t decay_frames[16] = {1, 2, 3, 5, 7, 11, 13, 15, 18, 45, 90, 144, 180, 255, 255, 255};

static const step_t seq_click[] = {{.freq = G3, .frames = 0}};
static const step_t seq_flag_on[] = {{.freq = C5, .frames = 3}, //
                                     {.freq = G5, .frames = 0}};
static const step_t seq_flag_off[] = {{.freq = G5, .frames = 3}, //
                                      {.freq = C5, .frames = 0}};
static const step_t seq_start[] = {{.freq = C5, .frames = 4}, //
                                   {E5, 4},                   //
                                   {G5, 4},                   //
                                   {C6, 0}};
static const step_t seq_win[] = {{.freq = G4, .frames = 5}, //
                                 {C5, 5},                   //
                                 {E5, 5},                   //
                                 {G5, 12},                  //
                                 {E5, 5},                   //
                                 {C6, 0}};
static const step_t seq_boom[] = {{.freq = 1200, .frames = 0}};
static const step_t seq_lose[] = {{.freq = REST, .frames = 15}, //
                                  {E4, 15},                     //
                                  {DS4, 15},                    //
                                  {D4, 15},                     //
                                  {CS4, 0}};

static const step_t *sequence_ptr[8];
static const instrument_t *sequence_ins[8];
static uint8_t sequence_timer[8];
static uint8_t gate_timer[8];

#define GATE_ADDR(ch) (XRAM_PSG + (ch) * 8 + offsetof(psg_channel_t, pan_gate))

void audio_init()
{
    xreg_ria_psg(XRAM_PSG);
}

static void play_step(uint8_t channel_index)
{
    const step_t *step = sequence_ptr[channel_index];
    const instrument_t *instrument = sequence_ins[channel_index];

    if (step->freq != REST)
    {
        psg_channel_t channel;

        channel.freq = step->freq;
        channel.duty = instrument->duty;
        channel.attack = instrument->attack;
        channel.decay = instrument->decay;
        channel.release_wave = instrument->release;
        channel.pan_gate = PSG_GATE;
        channel.reserved = 0;

        xram0_poke8(GATE_ADDR(channel_index), 0); /* stop previous sound */
        xram0_write(XRAM_PSG + channel_index * 8, &channel, sizeof(channel));

        const uint16_t timer = attack_frames[instrument->attack & 0x0F] + decay_frames[instrument->decay & 0x0F];
        gate_timer[channel_index] = timer > 255 ? 255 : (uint8_t)timer;
    }

    if (step->frames == 0)
    {
        sequence_ptr[channel_index] = NULL;
    }
    else
    {
        sequence_timer[channel_index] = step->frames;
        sequence_ptr[channel_index] = step + 1;
    }
}

static void play(uint8_t channel_index, const instrument_t *instrument, const step_t *sequence)
{
    sequence_ins[channel_index] = instrument;
    sequence_ptr[channel_index] = sequence;
    play_step(channel_index);
}

void play_press()
{
    play(0, &ins_click, seq_click);
}

void play_put_flag()
{
    play(1, &ins_flag_put, seq_flag_on);
}

void play_remove_flag()
{
    play(1, &ins_flag_remove, seq_flag_off);
}

void play_start()
{
    play(2, &ins_start, seq_start);
}

void play_win()
{
    play(2, &ins_win, seq_win);
}

void play_lose()
{
    play(4, &ins_boom, seq_boom);
    play(2, &ins_lose, seq_lose);
}

void audio_update()
{
    for (uint8_t ch = 0; ch < 8; ++ch)
    {
        if (sequence_ptr[ch])
        {
            if (--sequence_timer[ch] == 0)
            {
                play_step(ch);
            }
        }

        if (gate_timer[ch] && --gate_timer[ch] == 0)
        {
            const uint16_t a = XRAM_PSG + ch * 8 + offsetof(psg_channel_t, pan_gate);
            xram0_poke8(a, xram0_peek8(a) & ~PSG_GATE);
        }
    }
}
