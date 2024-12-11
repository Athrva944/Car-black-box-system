

#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "adc.h"
#include "timer0.h"
#include "ext_eeprom.h"
#include "i2c.h"
#include "ds1307.h"
#include "uart.h"

/* 
 * Function: init_config
 * ----------------------
 * Initializes various peripherals and modules used in the project.
 * 
 * This function sets up the matrix keypad, character LCD display (CLCD),
 * analog-to-digital converter (ADC), I2C communication, Timer0, DS1307 real-time
 * clock (RTC), and UART communication.
 */
void init_config() {
    init_matrix_keypad();
    init_clcd();
    init_adc();
    init_i2c();
    init_timer0();
    init_ds1307();
    init_uart();
}

// Global variables
unsigned char main_f = 0, menu_f = 0, key, index = 0;
unsigned short int flag, overflow = 0, lap = 0;
unsigned char arr_eeprom[10];
unsigned short int adc_reg_val, adc_val1, adc_val2;

unsigned char clock_reg[3];

// Event codes for different operations
char event[12][3] = {"ON", "GR", "GN", "G1", "G2", "G3", "C ", "DL", "CL", "ST", "VL", "CP"};

// Array to store formatted time
unsigned char time[9];

/* 
 * Function: store_in_ext_eeprom
 * ------------------------------
 * Stores the current time, event information, and ADC value in the external EEPROM.
 * 
 * This function formats the time, event code, and ADC value into an array and writes
 * each element to the external EEPROM. It also handles lap and overflow conditions
 * to manage the EEPROM write address.
 */
void store_in_ext_eeprom() {
    // Format time and event data for EEPROM
    arr_eeprom[0] = time[0];
    arr_eeprom[1] = time[1];
    arr_eeprom[2] = time[3];
    arr_eeprom[3] = time[4];
    arr_eeprom[4] = time[6];
    arr_eeprom[5] = time[7];

    arr_eeprom[6] = event[index][0];
    arr_eeprom[7] = event[index][1];

    arr_eeprom[8] = adc_reg_val / 10 + 48;
    arr_eeprom[9] = adc_reg_val % 10 + 48;
    arr_eeprom[10] = '\0'; // Null-terminate the string

    // Write formatted data to external EEPROM
    for (char i = 0; i < 10; i++) {
        write_ext_eeprom(lap * 10 + i, arr_eeprom[i]);
    }
    lap++;
    if (lap == 10) {
        lap = 0;
        overflow = 1;
    }
}

/* 
 * Function: get_time
 * -------------------
 * Retrieves the current time from the DS1307 RTC and formats it into a string.
 * 
 * The time is read from the RTC registers and converted into a human-readable
 * string format "HH:MM:SS".
 */
void get_time(void) {
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    // Format the hour part
    if (clock_reg[0] & 0x40) {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    } else {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    time[2] = ':';
    // Format the minute part
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
    time[4] = '0' + (clock_reg[1] & 0x0F);
    time[5] = ':';
    // Format the second part
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
    time[7] = '0' + (clock_reg[2] & 0x0F);
    time[8] = '\0'; // Null-terminate the string
}

/* 
 * Function: main
 * ---------------
 * Main execution loop for the application.
 * 
 * Initializes the system, retrieves the current time, and then enters an infinite loop
 * where it handles various operations based on switch inputs. It manages different states
 * including dashboard, password entry, menu navigation, and specific menu functions.
 */
void main(void) {
    init_config(); // Initialize peripherals and modules
    get_time(); // Get and store time at startup
    if (index == 0)
        store_in_ext_eeprom(); // Initial EEPROM storage

    while (1) {
        get_time(); // Continuously update time

        key = read_switches(STATE_CHANGE); // Read key press from matrix keypad

        // Handle switch inputs for various functions
        if (key == MK_SW1) {
            CLEAR_DISP_SCREEN;
            index = 6;
            store_in_ext_eeprom();
        }
        if (key == MK_SW2) {
            CLEAR_DISP_SCREEN;
            if (index < 5 && index >= 0)
                index++;
            else if (index == 5)
                index = 5;
            store_in_ext_eeprom();
        }
        if (key == MK_SW3) {
            CLEAR_DISP_SCREEN;
            if (index < 6 && index > 1)
                index--;
            store_in_ext_eeprom();
        }
        if (index == 6) {
            if (key == MK_SW2) {
                index = 2;
                store_in_ext_eeprom();
            }
            if (key == MK_SW3) {
                index = 2;
                store_in_ext_eeprom();
            }
        }

        if (key == MK_SW4 && main_f == 0) {
            flag = 1;
            CLEAR_DISP_SCREEN;
            main_f = PASSWORD; // Transition to password entry state
        }

        // State machine handling for different modes
        if (main_f == DASHBOARD) {
            dashboard(); // Handle dashboard display
        } else if (main_f == PASSWORD) {
            password(key); // Handle password entry
        } else if (main_f == MENU) {
            menu(key); // Handle menu navigation
        } else if (main_f == MENU_ENTER) {
            if (menu_f == VIEWLOG) {
                view_log(key); // View logs
            } else if (menu_f == DOWNLOADLOG) {
                download_log(); // Download logs
            } else if (menu_f == CLEARLOG) {
                clear_log(key); // Clear logs
            } else if (menu_f == SETTIME) {
                settime(key); // Set time
            } else if (menu_f == CHANGEPASS) {
                change_pass(key); // Change password
            }
        }
    }
    return; // Should never reach here
}
