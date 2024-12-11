

#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "ext_eeprom.h"

// External variables
extern char main_f;
extern unsigned char index;
extern char event[12][3];

/*
 * Function: change_pass
 * ----------------------
 * Handles the process of changing the password, including user input and validation.
 * 
 * Parameters:
 *     key - The key pressed on the matrix keypad.
 */
void change_pass(char key) {
    static unsigned int blink, count; // Blinking effect and input character count
    static int k; // Index for input arrays
    static char input1[4], input2[4]; // Arrays to store the new and re-entered password
    static char re_flag; // Flag to check if user is re-entering password
    static unsigned int wait1 = 100, wait2 = 100; // Timers for waiting before changing screens

    // Display prompts based on the stage of password entry
    if (count < 4) {
        if (!re_flag)
            clcd_print("Enter new passwd:", LINE1(0)); // Prompt for new password
        else
            clcd_print("Re-enter passwd: ", LINE1(0)); // Prompt for re-entered password
    }

    // Blinking effect on LCD every 250 cycles
    if (blink++ == 250) {
        clcd_putch(0x5F, LINE2(count)); // Display blinking character on LCD
    } else if (blink == 500) {
        clcd_putch(0x20, LINE2(count)); // Display space to stop blinking
        blink = 0; // Reset blink counter
    }

    // Handle keypad input
input:
    if (key != ALL_RELEASED && count < 4) {
        if (key == MK_SW4) {
            if (!re_flag)
                input1[k++] = '0'; // Record '0' in input array
            else
                input2[k++] = '0'; // Record '0' in re-entered password array
            clcd_putch(0x2A, LINE2(count)); // Display '*' on LCD
            count++; // Increment count of entered characters
        } else if (key == MK_SW5) {
            if (!re_flag)
                input1[k++] = '1'; // Record '1' in input array
            else
                input2[k++] = '1'; // Record '1' in re-entered password array
            clcd_putch(0x2A, LINE2(count)); // Display '*' on LCD
            count++; // Increment count of entered characters
        }
    } else if (count >= 4 && !re_flag) {
        re_flag = 1; // Switch to re-entering password phase
        count = 0; // Reset character count
        k = 0; // Reset index for input arrays
        CLEAR_DISP_SCREEN; // Clear the display
        goto input; // Go to input handling for re-entered password
    } else if (count >= 4) {
        // Validate re-entered password
        for (k = 0; k < 4; k++) {
            if (input2[k] != input1[k]) // If mismatch found
                break; // Exit loop
        }

        if (k == 4) { // If all characters match
            clcd_print("Password change ", LINE1(0)); // Display success message
            clcd_print("  SUCCESSFUL   ", LINE2(0)); // Display success message

            // Store new password in external EEPROM
            write_ext_eeprom(200, input1[0]);
            write_ext_eeprom(201, input1[1]);
            write_ext_eeprom(202, input1[2]);
            write_ext_eeprom(203, input1[3]);

            if (!wait1--) { // Wait before returning to menu
                re_flag = 0; // Reset re-enter flag
                count = 0; // Reset count
                k = 0; // Reset index
                CLEAR_DISP_SCREEN; // Clear the display
                wait1 = 100; // Reset wait timer
                main_f = MENU; // Set main function state to MENU
            }

            index = 11; // Set index for storing log
            store_in_ext_eeprom(); // Store log in EEPROM

        } else { // If passwords do not match
            clcd_print("   Enter same   ", LINE1(0)); // Prompt for matching passwords
            clcd_print("  password     ", LINE2(0)); // Prompt for matching passwords

            if (!wait2--) { // Wait before returning to menu
                main_f = MENU; // Set main function state to MENU
                re_flag = 0; // Reset re-enter flag
                count = 0; // Reset count
                k = 0; // Reset index
                CLEAR_DISP_SCREEN; // Clear the display
                wait2 = 100; // Reset wait timer
            }
        }
    }
}