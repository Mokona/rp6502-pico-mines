#include "xram.h"
#include <rp6502.h>
#include <stdio.h>
#include <string.h>

#include "hidkeys.h"

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

int main(void)
{
    puts("Hello, world!");

    keyboard_t kb;
    tablet_pointer_t tab;

    unsigned x = 0, y = 0;

    xreg_ria_keyboard(XRAM_KEYBOARD);
    xreg_ria_tablet(XRAM_TABLET);

    while (true)
    {
        xram0_read(&kb, XRAM_KEYBOARD, sizeof(kb));
        if (KEYBOARD_PRESSED(kb.keys, HID_KEY_SPACE))
        {
            puts("Hop!");
        }

        tablet_read(&tab);

        const unsigned old_x = x;
        const unsigned old_y = y;
        x = (tab.contact.yx_hi & 0x0F) << 8 | tab.contact.x_lo;
        y = (tab.contact.yx_hi >> 4) << 8 | tab.contact.y_lo;

        if (old_x != x || old_y != y)
        {
            printf("x=%d y=%d\n", old_x, old_y);
        }
    }
}
