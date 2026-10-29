#include "input_actions.h"

#include "hidkeys.h"
#include "xram.h"

#include <stdio.h>
#include <string.h>

static keyboard_t kb;
static tablet_pointer_t tab;
static gamepad_player_t p1;

typedef struct
{
    bool previous_space_bar_pressed;
    bool previous_alternate_pressed;
    bool previous_left_pressed;
} keyboard_previous_t;

static keyboard_previous_t keyboard_previous;

static actions_t actions;

void input_action_init()
{
    xreg_ria_keyboard(XRAM_KEYBOARD);
    xreg_ria_tablet(XRAM_TABLET);
    xreg_ria_gamepad(XRAM_GAMEPAD);

    memset(&actions, 0, sizeof(actions_t));
    memset(&keyboard_previous, 0, sizeof(keyboard_previous_t));
}

// Read the block again to check that the host did not write it during the
// read. Writes are at least 1 ms apart, so a third read will not overlap one.
static void tablet_read(tablet_pointer_t *tablet)
{
    tablet_pointer_t check;

    xram0_read(tablet, XRAM_TABLET, sizeof(tablet_pointer_t));
    xram0_read(&check, XRAM_TABLET, sizeof(tablet_pointer_t));
    if (memcmp(tablet, &check, sizeof(tablet_pointer_t)) != 0)
    {
        xram0_read(tablet, XRAM_TABLET, sizeof(tablet_pointer_t));
    }
}

const actions_t *input_action_update()
{
    // Keyboard
    xram0_read(&kb, XRAM_KEYBOARD, sizeof(kb));

    const bool space_bar_pressed = KEYBOARD_PRESSED(kb.keys, HID_KEY_SPACE);
    actions.just_pressed_action = space_bar_pressed && !keyboard_previous.previous_space_bar_pressed;
    keyboard_previous.previous_space_bar_pressed = space_bar_pressed;

    const bool alternate_pressed = KEYBOARD_PRESSED(kb.keys, HID_KEY_X);
    actions.just_pressed_alternate = alternate_pressed && !keyboard_previous.previous_alternate_pressed;
    keyboard_previous.previous_alternate_pressed = alternate_pressed;

    const bool left_pressed = KEYBOARD_PRESSED(kb.keys, HID_KEY_ARROW_LEFT);
    actions.just_pressed_left = left_pressed && !keyboard_previous.previous_left_pressed;
    keyboard_previous.previous_left_pressed = left_pressed;

    // Tablet
    tablet_read(&tab);

    const uint16_t old_x = actions.pointer_x;
    const uint16_t old_y = actions.pointer_y;
    const uint16_t x = (tab.contact.yx_hi & 0x0F) << 8 | tab.contact.x_lo;
    const uint16_t y = (tab.contact.yx_hi >> 4) << 8 | tab.contact.y_lo;

    if (old_x != x || old_y != y)
    {
        actions.pointer_moved = true;
        actions.pointer_x = x;
        actions.pointer_y = y;
    }
    else
    {
        actions.pointer_moved = false;
    }

    const uint8_t old_tablet_status = actions.tablet_status;
    actions.just_pressed_action |= (tab.contact.flags & TABLET_FLAG_LEFT) && !(old_tablet_status & TABLET_FLAG_LEFT);
    actions.just_pressed_alternate |= (tab.contact.flags & TABLET_FLAG_RIGHT) && !(old_tablet_status & TABLET_FLAG_RIGHT);
    actions.tablet_status = tab.contact.flags;

    // Gamepad
    const uint8_t old_dpad = actions.dpad;
    xram0_read(&p1, XRAM_GAMEPAD, sizeof(p1));
    if (p1.dpad & GAMEPAD_FEAT_CONNECTED)
    {
        actions.just_pressed_left |= (p1.dpad & GAMEPAD_DPAD_LEFT) && !(old_dpad & GAMEPAD_DPAD_LEFT);
        actions.just_pressed_action |= (p1.dpad & GAMEPAD_BTN0_A) && !(old_dpad & GAMEPAD_BTN0_A);
    }

    return &actions;
}