/* RP6502 XRAM Mapping */

#ifndef XRAM_H
#define XRAM_H

#include <rp6502.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Insert xram.h snippets from docs to build your system. */
/* https://picocomputer.github.io/sdk.html#xram-memory-map */

typedef struct
{
    uint8_t keys[32];
} keyboard_t; /* layout */

#define xreg_ria_keyboard(...) xreg(0x0, 0x0, 0x00, __VA_ARGS__)
#define xreg_ria_tablet(...) xreg(0x0, 0x0, 0x03, __VA_ARGS__)

#define KEYBOARD_PRESSED(keys, code) ((keys)[(code) >> 3] & (1 << ((code) & 7)))

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


/* Insert XRAM structs from those snippets here along with */
/* your own custom usage to define your XRAM memory map. */
typedef struct
{
    keyboard_t keyboard;
    tablet_t tablet;
} xram_layout_t;

/* Define constants to be your XRAM addresses. */
#define XRAM_KEYBOARD offsetof(xram_layout_t, keyboard)
#define XRAM_TABLET offsetof(xram_layout_t, tablet)

#endif
