
#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "timer0.h"
#include "matrix_keypad.h"
#include "ext_eeprom.h"
#include <string.h>

// Static and external variables
unsigned static int j; // For blinking cursor logic
static unsigned int attempt = 2; // Number of remaining attempts
unsigned static char index1 = 0; // Index for password input
extern unsigned short int flag; // External flag
extern unsigned char main_f; // External main function state
short int count_3min = 180, count_5sec = 5; // Timers for block and delay
char once = 1; // Flag to handle blocking logic

/*
 * Function: password
 * -------------------
 * Handles password entry, verification, and blocking logic.
 * key: The key press from the matrix keypad.
 */
void password(char key) {
    // Array to store the entered password
    static char user_password[4];
    
     /*write_ext_eeprom(200,'0');
    write_ext_eeprom(201,'1');
    write_ext_eeprom(202,'0');
    write_ext_eeprom(203,'1');*/
    
    // Blinking cursor logic
    if (index1 < 4) {
        if (j++ < 250)
            clcd_putch('_', LINE2(index1)); // Display blinking cursor
        else if (j < 500)
            clcd_putch(' ', LINE2(index1)); // Hide cursor
        else
            j = 0; // Reset blink counter

        // Display prompt and handle timeout
        clcd_print("ENTER PASSWORD ", LINE1(0));
        if (count_5sec == 0) {
            CLEAR_DISP_SCREEN;
            main_f = DASHBOARD; // Go back to dashboard if timeout
        }

        // Handle key press for password entry
        if (key != ALL_RELEASED) {
            count_5sec = 5; // Reset timer on key press
            if (key == MK_SW4 && !flag) {
                user_password[index1] = '0'; // Record '0'
                clcd_putch('*', LINE2(index1)); // Display '*' for input
                index1++;
            }
            
            flag = 0; // Reset flag

            if (key == MK_SW5) {
                user_password[index1] = '1'; // Record '1'
                clcd_putch('*', LINE2(index1)); // Display '*' for input
                index1++;
            }
        }
    } else if (index1 >= 4) { // Check password after 4 digits are entered
        int i;
        for (i = 0; i < 4; i++) {
            if (read_ext_eeprom(200 + (1 * i)) != user_password[i])
                break; // Exit loop if mismatch found
        }

        if (i == 4) { // Password matches
            clcd_print("Password correct", LINE1(0));
            clcd_print("  Welcome user  ", LINE2(0));
            for (unsigned long int l = 500000; l--;) ; // Blocking delay
            CLEAR_DISP_SCREEN;
            index1 = 0; // Reset index
            main_f = MENU; // Go to menu
            attempt = 2; // Reset attempts
            return;
        } else if (attempt >= 1) { // Password mismatch but attempts left
            clcd_print("Wrong password", LINE1(0));
            clcd_print("attempt left=", LINE2(0));
            attempt--; // Decrement attempts
            index1 = 0; // Reset index
            clcd_putch((attempt + '1'), LINE2(14)); // Display remaining attempts

            for (unsigned long int l = 500000; l--;) ; // Blocking delay

            CLEAR_DISP_SCREEN;
        } else if (attempt == 0) { // No attempts left
            if (once == 1) {
                CLEAR_DISP_SCREEN;
                count_3min = 180; // Reset block timer
                once = 0; // Set flag to indicate blocking
            }

            clcd_print(" YOU ARE BLOCKED", LINE1(0));
            clcd_print("  FOR  ", LINE2(0));

            // Display block time
            clcd_putch((count_3min / 100) % 10 + 48, LINE2(6));
            clcd_putch((count_3min / 10) % 10 + 48, LINE2(7));
            clcd_putch((count_3min % 10) + 48, LINE2(8));

            clcd_print(" SEC  ", LINE2(9));

            if (count_3min == 0) { // Reset block after time expires
                CLEAR_DISP_SCREEN;
                count_3min = 180; // Reset block timer
                index1 = 0; // Reset index
                attempt = 2; // Reset attempts
                once = 1; // Reset flag
                j = 0; // Reset blink counter
            }
        }
    }
}
