#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <timers.h>
#include <usart.h>

/* Allocates the system ticks counter (milliseconds since boot). */
volatile uint32_t systicks = 0;

/* ======================================== Timer0 ======================================== */


/* Initialize Timer0 for CTC mode. */
void Timer0_init_ctc(void)
{
    // Clear registers
    TCCR0A = 0;
    TCCR0B = 0;

    // Set CTC mode
    TCCR0A |= (1 << WGM01);

    // Set prescaler to 256
    TCCR0B |= (1 << CS02);

    // Enable both Compare A and Compare B interrupts
    TIMSK0 |= (1 << OCIE0A);  // For PD5
}

ISR(TIMER0_COMPA_vect)
{
    PORTD ^= (1 << PD5);  // Toggle buzzer 1
}

/* ======================================== Timer1 ======================================== */

/*
 * Initializes Timer1 for systick interrupt once every 1ms,
 * plus any custom behavior you require.
 */
void Timer1_init_systicks(void)
{
    /* Clear previous settings */
    TCCR1A = 0;
    TCCR1B = 0;

    /* Set CTC mode */
    TCCR1B |= (1 << WGM12);

    /* Set prescaler to 8 */
    TCCR1B |= (1 << CS11);

    /* Activate Compare A interrupt */
    TIMSK1 |= (1 << OCIE1A); 

    /* 12MHz / 8 => 1500 kHz */
    OCR1A = 1500;
}

ISR(TIMER1_COMPA_vect)
{
    /* Will get called [almost] once every 1ms! */
    systicks++;
    /* Note: the timer is in Clear Timer on Compare Match mode, so it will
     * automatically reset itself back to 0! */
}


/* ======================================== Timer2 ======================================== */

/* Initialize Timer2 for CTC mode (8 bits). */
void Timer2_init_ctc(void)
{
    /* Clear previous settings */
    TCCR2A = 0;
    TCCR2B = 0;

    /* Set CTC Mode */
    TCCR2A |= (1 << WGM21);

    /* Prescaler to 128 */
    TCCR2B |= (1 << CS22) | (1 << CS21);

    // Enable Compare A
    TIMSK2 |= (1 << OCIE2A);  // For PD6
}


ISR(TIMER2_COMPA_vect)
{
    PORTD ^= (1 << PD6);  // Toggle buzzer 2
}
