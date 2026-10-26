/* RP6502 XRAM Mapping */

#ifndef XRAM_H
#define XRAM_H

#include <rp6502.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Keyboard
typedef struct
{
    uint8_t keys[32];
} keyboard_t; /* layout */

#define xreg_ria_keyboard(...) xreg(0x0, 0x0, 0x00, __VA_ARGS__)

#define KEYBOARD_PRESSED(keys, code) ((keys)[(code) >> 3] & (1 << ((code) & 7)))

// Tablet
#define TABLET_CONTACTS 8

#define TABLET_STATUS_HOST_CURSOR 0x01

#define TABLET_FLAG_LEFT 0x01
#define TABLET_FLAG_RIGHT 0x02
#define TABLET_FLAG_MIDDLE 0x04
#define TABLET_FLAG_BACKWARD 0x08
#define TABLET_FLAG_FORWARD 0x10
#define TABLET_FLAG_HOVER 0x80

#define TABLET_CURSOR_OFF 0
#define TABLET_CURSOR_ARROW 1
#define TABLET_CURSOR_CROSSHAIR 2
#define TABLET_CURSOR_IBEAM 3
#define TABLET_CURSOR_HAND 4
#define TABLET_CURSOR_RESIZE_EW 5
#define TABLET_CURSOR_RESIZE_NS 6

#define xreg_ria_tablet(...) xreg(0x0, 0x0, 0x03, __VA_ARGS__)

typedef struct
{
    uint8_t flags;
    uint8_t y_lo;
    uint8_t x_lo;
    uint8_t yx_hi;
} tablet_contact_t;

typedef struct
{
    uint8_t control;
    uint8_t status;
    uint8_t wheel;
    uint8_t pan;
    tablet_contact_t contact;
} tablet_pointer_t;

typedef struct
{
    uint8_t control;
    uint8_t status;
    uint8_t wheel;
    uint8_t pan;
    tablet_contact_t contact[TABLET_CONTACTS];
} tablet_t; /* layout */

#define xreg_ria_tablet(...) xreg(0x0, 0x0, 0x03, __VA_ARGS__)

// Gamepad
#define GAMEPAD_PLAYERS 4

#define GAMEPAD_DPAD_UP 0x01
#define GAMEPAD_DPAD_DOWN 0x02
#define GAMEPAD_DPAD_LEFT 0x04
#define GAMEPAD_DPAD_RIGHT 0x08

#define GAMEPAD_FEAT_TYPE_MASK 0x30
#define GAMEPAD_TYPE_UNKNOWN 0x00
#define GAMEPAD_TYPE_WESTERN 0x10
#define GAMEPAD_TYPE_EASTERN 0x20
#define GAMEPAD_TYPE_PLAYSTATION 0x30
#define GAMEPAD_FEAT_STICKS 0x40
#define GAMEPAD_FEAT_CONNECTED 0x80

#define GAMEPAD_LSTICK_UP 0x01
#define GAMEPAD_LSTICK_DOWN 0x02
#define GAMEPAD_LSTICK_LEFT 0x04
#define GAMEPAD_LSTICK_RIGHT 0x08
#define GAMEPAD_RSTICK_UP 0x10
#define GAMEPAD_RSTICK_DOWN 0x20
#define GAMEPAD_RSTICK_LEFT 0x40
#define GAMEPAD_RSTICK_RIGHT 0x80

#define GAMEPAD_BTN0_A 0x01
#define GAMEPAD_BTN0_B 0x02
#define GAMEPAD_BTN0_C 0x04
#define GAMEPAD_BTN0_X 0x08
#define GAMEPAD_BTN0_Y 0x10
#define GAMEPAD_BTN0_Z 0x20
#define GAMEPAD_BTN0_L1 0x40
#define GAMEPAD_BTN0_R1 0x80

#define GAMEPAD_BTN1_L2 0x01
#define GAMEPAD_BTN1_R2 0x02
#define GAMEPAD_BTN1_SELECT 0x04
#define GAMEPAD_BTN1_START 0x08
#define GAMEPAD_BTN1_HOME 0x10
#define GAMEPAD_BTN1_L3 0x20
#define GAMEPAD_BTN1_R3 0x40

#define xreg_ria_gamepad(...) xreg(0x0, 0x0, 0x02, __VA_ARGS__)

typedef struct
{
    uint8_t dpad;
    uint8_t sticks;
    uint8_t btn0;
    uint8_t btn1;
    int8_t lx;
    int8_t ly;
    int8_t rx;
    int8_t ry;
    uint8_t l2;
    uint8_t r2;
} gamepad_player_t;

typedef struct
{
    gamepad_player_t player[GAMEPAD_PLAYERS];
} gamepad_t; /* layout */

// XRAM LAYOUR
typedef struct
{
    keyboard_t keyboard;
    tablet_t tablet;
    gamepad_player_t gamepad;
} xram_layout_t;

/* Define constants to be your XRAM addresses. */
#define XRAM_KEYBOARD offsetof(xram_layout_t, keyboard)
#define XRAM_TABLET offsetof(xram_layout_t, tablet)
#define XRAM_GAMEPAD offsetof(xram_layout_t, gamepad)

#endif
