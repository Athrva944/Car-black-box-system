

#include <xc.h>
#include "main.h"
#include "i2c.h"
#include "ext_eeprom.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "isr.h"

// External variables
extern unsigned char main_f;
extern unsigned short int lap, overflow;

// Static variables
unsigned int view = 0; // Current log view index

/*
 * Function: view_log
 * -------------------
 * Displays the log entry on the CLCD and handles navigation through the logs.
 * key: The key press from the matrix keypad.
 */
void view_log(char key) {
    static int count1 = 0; // Timer for automatic return to menu
    static char v_key; // Variable to store the key press

    // Read the current key state
    v_key = read_switches(LEVEL_CHANGE);

    // Display log entry on the CLCD
    clcd_print("#  LOGS : ", LINE1(0));

    clcd_putch(view + '0', LINE2(0)); // Display log index
    clcd_putch(read_ext_eeprom(view * 10 + 0), LINE2(2)); // Display hour
    clcd_putch(read_ext_eeprom(view * 10 + 1), LINE2(3)); // Display minute
    clcd_putch(':', LINE2(4)); // Separator

    clcd_putch(read_ext_eeprom(view * 10 + 2), LINE2(5)); // Display second
    clcd_putch(read_ext_eeprom(view * 10 + 3), LINE2(6)); // Display speed
    clcd_putch(':', LINE2(7)); // Separator

    clcd_putch(read_ext_eeprom(view * 10 + 4), LINE2(8)); // Display event
    clcd_putch(read_ext_eeprom(view * 10 + 5), LINE2(9)); // Display event

    clcd_putch(read_ext_eeprom(view * 10 + 6), LINE2(11)); // Display additional data
    clcd_putch(read_ext_eeprom(view * 10 + 7), LINE2(12)); // Display additional data

    clcd_putch(read_ext_eeprom(view * 10 + 8), LINE2(14)); // Display additional data
    clcd_putch(read_ext_eeprom(view * 10 + 9), LINE2(15)); // Display additional data

    // Navigate logs with key presses
    if (key == MK_SW5 && view < 9 && (view < (lap - 1) || overflow == 1)) {
        view++; // Move to the next log
    } else if (key == MK_SW4 && view > 0) {
        view--; // Move to the previous log
    }

    // Automatic return to menu if no key press
    if (v_key == MK_SW5) {
        count1++;
    } else if (count1 > 1 && count1 < 200) {
        count1 = 0; // Reset timer if key is released
    }

    if (count1 == 200) {
        count1 = 0; // Reset timer
        main_f = MENU; // Go back to menu
        CLEAR_DISP_SCREEN; // Clear display
    }

    return;
}