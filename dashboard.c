
#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "adc.h"
#include "ds1307.h"

// External variables
extern unsigned char time[9]; // Current time string
extern unsigned short int adc_reg_val, adc_val1, adc_val2; // ADC values

extern unsigned char main_f, menu_f; // Main function state, menu state
extern unsigned char index; // Current index for event

extern char event[12][3]; // Array of event codes

/*
 * Function: dashboard
 * --------------------
 * Updates the dashboard display with the current time, event, and speed.
 */

void dashboard() {
    // Display static labels on the LCD
    clcd_print("TIME", LINE1(0)); // Label for time display
    clcd_print("EV", LINE1(10)); // Label for event display
    clcd_print("SP", LINE1(13)); // Label for speed display

    // Display the current time
    clcd_print(time, LINE2(0)); // Print time string on the second line of the LCD

    // Display the current event
    clcd_print(event[index], LINE2(10)); // Print event code based on index

    // Speed calculation and display
    adc_reg_val = read_adc(CHANNEL4) / 10.33; // Read and convert ADC value to speed (0-99 range)
    adc_val1 = adc_reg_val / 10; // Extract tens place
    adc_val2 = adc_reg_val % 10; // Extract ones place

    // Display the speed on the LCD
    clcd_putch(adc_val1 + 48, LINE2(13)); // Print tens place of speed
    clcd_putch(adc_val2 + 48, LINE2(14)); // Print ones place of speed
}