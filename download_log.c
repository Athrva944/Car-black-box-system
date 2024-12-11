

#include <xc.h>
#include "main.h"
#include "uart.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "ext_eeprom.h"

// External variables
extern unsigned char main_f; // Main function state
extern unsigned short int overflow, lap; // Overflow and lap counters
extern unsigned char index; // Current index for event
char event[12][3]; // Array of event codes
unsigned int prev_event; // Previous event index

/*
 * Function: download_log
 * -----------------------
 * Downloads and displays log data from external EEPROM.
 */
void download_log() {
    char j = 0; // Index for log data array
    char log[11], curr_log; // Array for storing log data and current log entry
    static char once; // Flag to ensure the log is downloaded only once

    if (!once) {
        prev_event = index; // Save current index
        index = 7; // Set index for log download
        store_in_ext_eeprom(); // Store current state in EEPROM

        puts("Index  Time  Event  Speed\n\r"); // Print column headers

        // Loop through log entries
        for (curr_log = 0; curr_log < (overflow ? 10 : lap); curr_log++) {
            // Read log entry from EEPROM
            for (char i = 0; i < 10; i++) {
                log[i] = read_ext_eeprom(i + (curr_log * 10)); // Read each byte of log entry
            }
            log[10] = '\0'; // Null-terminate the log string

            // Print log entry
            for (char i = 0; i < 16; i++) {
                if (i == 0)
                    putch(curr_log + '0'); // Print log index
                else if (i == 1)
                    putch(' '); // Print space
                else if (i == 10)
                    puts("  "); // Print space for event
                else if (i == 13)
                    puts("    "); // Print space for speed
                else if (i == 4 || i == 7)
                    putch(':'); // Print colon for time format
                else
                    putch(log[j++]); // Print log data
            }
            puts("\n\r"); // Newline after each log entry
            j = 0; // Reset log index for next entry
        }

        // Display success message on CLCD
        clcd_print("  Download Log  ", LINE1(0)); // Success message
        clcd_print("   SUCCESSFULL  ", LINE2(0)); // Success message
        once = 1; // Set flag to prevent re-downloading
    }

    // Handle key press for exiting the log download
    char key = read_switches(LEVEL_CHANGE); // Read key press

    static short int wait = 0; // Timer for exiting the log download

    if (key == MK_SW5) {
        wait++; // Increment wait timer
        if (wait == 800) {
            wait = 0; // Reset wait timer
            CLEAR_DISP_SCREEN; // Clear the display
            main_f = MENU; // Return to menu state
        }
    }

    return;
}