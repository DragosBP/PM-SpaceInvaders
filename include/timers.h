#ifndef TIMERS_H_
#define TIMERS_H_

#include <inttypes.h>

/* 
 * Declares the system ticks counter (milliseconds since boot)
 * for use within other .c modules.
 */
extern volatile uint32_t systicks;
/* 
 * Useful macro to check whether a specific amount of time
 * passed since some given last event.
 */
#define SYSTICKS_PASSED(last_event, diff_amount) \
	((systicks - (last_event)) >= (diff_amount))


/* Initialize Timer0 for CTC mode (8 bits). */
void Timer0_init_ctc(void);

/*
 * Initializes Timer1 for systick interrupt once every 1ms,
 * plus any custom behavior you require.
 */
void Timer1_init_systicks(void);

/* Initialize Timer2 for Fast PWM mode (8 bits). */
void Timer2_init_ctc(void);

#endif // TIMERS_H_
