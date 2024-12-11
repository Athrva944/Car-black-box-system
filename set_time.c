

#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "ds1307.h"
#include "matrix_keypad.h"

// External variables
extern unsigned char index;
extern unsigned char time[9], main_f;
unsigned char pre_event, curr_event;

/*
 * Function: settime
 * -----------------
 * Handles setting the time on the display and updating the DS1307 RTC.
 * key: The key press from the matrix keypad.
 */
void settime(char key) {
    // Static variables for managing time setting logic
    static unsigned char hr, min, sec; // Hours, minutes, and seconds
    static unsigned char once = 0, field_flag = 2; // Flags for logic
    static unsigned int blink = 0; // Blinking cursor timer
    static int set_timer = 0, back_timer = 0, wait = 500; // Timers

    // Read key input from the keypad
    key = read_switches(LEVEL_CHANGE);

    if (!once) {
        // Initialize time values
        clcd_print("Time (HH:MM:SS ) ", LINE1(0));
        clcd_print(time, LINE2(0));
        hr = ((time[0] - '0') * 10) + (time[1] - '0'); // Extract hour
        min = ((time[3] - '0') * 10) + (time[4] - '0'); // Extract minute
        sec = ((time[6] - '0') * 10) + (time[7] - '0'); // Extract second
    }
    once = 1;

    // Blinking cursor logic
    if (blink++ == 250) {
        if (field_flag == 0) {
            clcd_print("  ", LINE2(0)); // Blink cursor in hour field
        } else if (field_flag == 1) {
            clcd_print("  ", LINE2(3)); // Blink cursor in minute field
        } else if (field_flag == 2) {
            clcd_print("  ", LINE2(6)); // Blink cursor in second field
        }
    } else if (blink == 500) {
        // Display time with cursor
        clcd_putch((hr / 10) + '0', LINE2(0));
        clcd_putch((hr % 10) + '0', LINE2(1));
        clcd_putch(':', LINE2(2));
        clcd_putch((min / 10) + '0', LINE2(3));
        clcd_putch((min % 10) + '0', LINE2(4));
        clcd_putch(':', LINE2(5));
        clcd_putch((sec / 10) + '0', LINE2(6));
        clcd_putch((sec % 10) + '0', LINE2(7));
        blink = 0; // Reset blink timer
    }

    // Increment time field based on key press
    if (key == MK_SW5) {
        back_timer++;
    } else if (back_timer > 1 && back_timer < 500) {
        if (field_flag == 0) {
            hr = (hr + 1) % 24; // Increment hours
        } else if (field_flag == 1) {
            min = (min + 1) % 60; // Increment minutes
        } else if (field_flag == 2) {
            sec = (sec + 1) % 60; // Increment seconds
        }
        back_timer = 0; // Reset back timer
    }

    // Exit time setting and return to menu
    if (back_timer >= 500) {
        back_timer = 0;
        once = 0;
        field_flag = 2; // Reset field flag to seconds
        wait = 500; // Reset wait timer
        main_f = MENU; // Go back to menu
        CLEAR_DISP_SCREEN; // Clear display
    }

    // Change time field based on key press
    if (key == MK_SW4) {
        set_timer++;
    } else if (set_timer > 1 && set_timer < 400) {
        field_flag = (field_flag + 1) % 3; // Change field flag
        set_timer = 0; // Reset set timer
    }

    // Save time and return to menu
    if (set_timer >= 400) {
        clcd_print("Time Set        ", LINE1(0));
        clcd_print("Successfully!   ", LINE2(0));

        if (!wait--) {
            if (curr_event <= 6)
                pre_event = curr_event; // Save previous event
            curr_event = 9; // Set current event
            index = 9; // Set index
            store_in_ext_eeprom(); // Store data in EEPROM

            // Write time to DS1307 RTC
            write_ds1307(HOUR_ADDR, (((hr / 10) << 4) | (hr % 10)));
            write_ds1307(MIN_ADDR, (((min / 10) << 4) | (min % 10)));
            write_ds1307(SEC_ADDR, (((sec / 10) << 4) | (sec % 10)));

            set_timer = 0; // Reset set timer
            once = 0; // Reset once flag
            field_flag = 2; // Reset field flag to seconds
            wait = 500; // Reset wait timer
            main_f = MENU; // Go back to menu
            CLEAR_DISP_SCREEN; // Clear display
        }
    }
}
