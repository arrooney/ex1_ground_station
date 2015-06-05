/*
 * time.h
 *
 *  Created on: 20/05/2010
 *      Author: Johan Christiansen
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>
#include <util/timestamp.h>

void clock_uptime(timestamp_t * time);
void clock_get_time(timestamp_t *);
void clock_set_time(timestamp_t *);
void clock_get_monotonic(timestamp_t * time);
uint64_t clock_get_nsec(void);

/**
 * Restore the system time from an external RTC clock.
 *
 * This clock's precision is typically less than a second
 * and should not be called other than from the system bootup.
 */
void clock_restore_time_from_rtc(void);

/**
 * Calculate the currect tick's fractional value, that is
 * how close are we to get another tick.
 * Returns a number between 0 and 1, this includes zero if the
 * timercounter was just reset, but the value 1 should never
 * be obtained since the counter should automatically reset when it reaches
 * its maximum value.
 *
 * The function is not included in the standard util library, but must be provided by
 * a platform specific function with direct access to the tick timer.
 * If you don't support this feature in your timer, just make a dummy function
 * that returns zero
 *
 * Todo: there may be an off by one error here.
 *
 * @return Tick timer's fractional value converted to nsecs
 */
extern uint32_t clock_get_subtick_nsec(void);

#endif /* CLOCK_H_ */
