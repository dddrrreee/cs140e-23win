#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

/**
 * Starts the watchdog timer for a specified duration in ticks.
 *
 * @param ticks   The number of timer ticks (each of which is 16us).
 */
void watchdog_start_ticks(unsigned ticks);

/**
 * Starts the watchdog timer for a specified duration in microseconds.
 *
 * @param us  The number of microseconds (truncated to increments of 16).
 */
void watchdog_start_us(unsigned us);

/**
 * Starts the watchdog timer for a specified duration in milliseconds.
 *
 * @param ms  The number of milliseconds.
 */
void watchdog_start_ms(unsigned ms);

/**
 * Starts the watchdog timer for a specified duration in seconds.
 *
 * @param ms  The number of seconds.
 */
void watchdog_start_sec(unsigned sec);

/**
 * Checks if the watchdog timer is currently active.
 *
 * @return  True if the watchdog is running, false if it's stopped.
 */
int watchdog_is_running(void);

/**
 * Stops the watchdog timer.  Does not reset the counter.
 */
void watchdog_stop(void);

/**
 * Resumes the watchdog timer.
 *
 * If the timer was not previously started and stopped, the behavior is
 * undefined (i.e., I don't know what the hardware will do, but it probably
 * depends on the state bootcode.bin initializes it to).
 */
void watchdog_resume(void);

/**
 * Gets the time remaining on the watchdog timer in ticks.
 *
 * @return  The number of ticks remaining.
 */
unsigned watchdog_get_time_left_ticks(void);

/**
 * Gets the time remaining on the watchdog timer in microseconds.
 *
 * @return  The number of microseconds remaining.
 */
unsigned watchdog_get_time_left_us(void);

/**
 * Gets the time remaining on the watchdog timer in milliseconds.
 *
 * @return  The number of milliseconds remaining.
 */
unsigned watchdog_get_time_left_ms(void);

/**
 * Gets the time remaining on the watchdog timer in seconds.
 *
 * @return  The number of seconds remaining.
 */
unsigned watchdog_get_time_left_sec(void);

/**
 * Sets the watchdog to the minimum time interval, in the hopes of triggering a
 * reboot.  This is a more dangerous version of `rpi_reboot()`, since it
 * doesn't drop to user mode or wait for the reboot to actually succeed.
 */
void watchdog_reboot(void);

/**
 * "Feeds" the watchdog.  Restores the timer to the last-set starting count, as
 * provided by a call to `watchdog_start_*`.  This can be useful if you have a
 * program that runs forever intentionally (e.g., a game with a main loop), but
 * you want to catch inner functions hanging.
 *
 * If you just want to wait for an external event to happen (user input, an
 * incoming signal), it's probably simpler to stop the watchdog before the
 * blocking call and resume it after.
 */
void watchdog_feed(void);

#endif
