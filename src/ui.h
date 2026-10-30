#ifndef MY_RP6502_PROJECT_UI_H
#define MY_RP6502_PROJECT_UI_H

#include <stdbool.h>
#include <stdint.h>

void draw_happy_face();
void draw_sad_face();
void draw_timer();
void draw_time(uint16_t time);
void draw_level(uint16_t level);
void draw_ui();
void draw_flags(uint8_t count);
bool is_hovering_gear(uint16_t x, uint16_t y);
bool is_hovering_face(uint16_t x, uint16_t y);

#endif // MY_RP6502_PROJECT_UI_H
