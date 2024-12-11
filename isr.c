#include <xc.h>
#include "main.h"
#include "timer0.h"

// Global variable to keep track of timer counts
unsigned int count;

// External variables to keep track of 3-minute and 5-second intervals
extern short int count_3min, count_5sec;

/*
 * Interrupt Service Routine (ISR)
 * -------------------------------
 * Handles Timer0 overflow interrupt.
 */
void __interrupt() isr(void) {
    // Check if Timer0 overflow interrupt flag is set
    if (TMR0IF) {
        // Adjust Timer0 register to maintain desired timing
        TMR0 = TMR0 + 8;

        // Increment the count variable
        if (count++ == 20000) // 20000 counts = 1 second at 8-bit timer rate
        {
            count = 0; // Reset count to zero

            // Decrement the 3-minute and 5-second counters
            count_3min--;
            count_5sec--;
        }

        // Clear Timer0 interrupt flag
        TMR0IF = 0;
    }
}