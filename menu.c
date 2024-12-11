

#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "matrix_keypad.h"

// Array to store menu options
unsigned char menu_log[5][16] = {
    "View log       ",
    "Download log   ",
    "Clear log      ",
    "Set time       ",
    "Change password"
};

// Static variables for menu logic
unsigned static int menu_wait1, menu_wait2; // Wait timers for menu navigation
unsigned static char menu_i, ar_i; // Menu index and animation flag

// External variables
extern char main_f, menu_f; // Main function state and menu function state

/*
 * Function: menu
 * --------------
 * Handles menu navigation and selection based on key presses.
 * @param key: The key press from the matrix keypad.
 */
void menu(char key) {
    key = read_switches(LEVEL_CHANGE); // Read the current key press

    // Toggle display animation based on ar_i flag
    if (!ar_i) {
        clcd_putch(0X7E, LINE1(0)); // Display character for the top line
        clcd_putch(' ', LINE2(0)); // Clear the bottom line
    } else {
        clcd_putch(' ', LINE1(0)); // Clear the top line
        clcd_putch(0X7E, LINE2(0)); // Display character for the bottom line
    }

    // Print current and next menu items on the LCD
    clcd_print(menu_log[menu_i], LINE1(1));
    clcd_print(menu_log[menu_i + 1], LINE2(1));

    // Handle key press for scrolling down
    if (key == MK_SW5) {
        menu_wait2++; // Increment down scroll timer
    } else if (menu_wait2 > 1 && menu_wait2 < 500) {
        // Scroll down if within timer range
        if (!ar_i) {
            ar_i = 1; // Set animation flag
        } else if (menu_i < 3) {
            menu_i++; // Increment menu index
        }
        menu_wait2 = 0; // Reset timer
    } else {
        menu_wait2 = 0; // Reset timer if key is not pressed
    }

    // Return to dashboard if down scroll timer reaches threshold
    if (menu_wait2 >= 500) {
        menu_i = 0; // Reset menu index
        ar_i = 0; // Reset animation flag
        CLEAR_DISP_SCREEN; // Clear display
        main_f = DASHBOARD; // Set main function state to DASHBOARD
    }

    // Handle key press for scrolling up
    if (key == MK_SW4) {
        menu_wait1++; // Increment up scroll timer
    } else if (menu_wait1 > 1 && menu_wait1 < 500) {
        // Scroll up if within timer range
        if (ar_i) {
            ar_i = 0; // Clear animation flag
        } else if (menu_i > 0) {
            menu_i--; // Decrement menu index
        }
        menu_wait1 = 0; // Reset timer
    } else {
        menu_wait1 = 0; // Reset timer if key is not pressed
    }

    // Confirm selection and enter menu if up scroll timer reaches threshold
    if (menu_wait1 >= 500) {
        menu_f = menu_i + ar_i; // Set menu function state based on selection
        main_f = MENU_ENTER; // Set main function state to MENU_ENTER
        ar_i = 0; // Reset animation flag
        CLEAR_DISP_SCREEN; // Clear display
    }
}