
#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "ext_eeprom.h"

// External variables
extern unsigned char main_f;
extern unsigned short int lap, overflow;
extern unsigned char index;
extern char event[12][3];

/*
 * Function: clear_log
 * --------------------
 * Clears the log data from external EEPROM and updates the display to indicate success.
 * 
 * Parameters:
 *     key - The key pressed on the matrix keypad (currently unused in this function).
 */

void clear_log(char key) {
    static int wait = 1000; // Timer for display update delay
    static char once; // Flag to ensure the log is only cleared once

    if (!once) {
        index = 8; // Set index to start clearing log entries
        overflow = 0; // Reset overflow flag
        lap = 0; // Reset lap counter
        store_in_ext_eeprom(); // Store updated EEPROM data
        clcd_print("   CLEAR LOG    ", LINE1(0)); // Display log clearing message
        clcd_print("  SUCCESSFULL   ", LINE2(0)); // Display success message
        once = 1; // Set flag to prevent repeated clearing
    }

    if (!wait--) { // Wait for the specified time
        once = 0; // Reset flag for next use
        wait = 1000; // Reset wait timer
        CLEAR_DISP_SCREEN; // Clear the display
        main_f = MENU; // Return to the menu state
    }
}