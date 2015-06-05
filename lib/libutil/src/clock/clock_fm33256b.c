#include <inttypes.h>
#include <stdint.h>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <util/clock.h>

#include "fm33256b.h"

#include <FreeRTOS.h>
#include <task.h>

void clock_get_time(timestamp_t * time) {

	struct fm33256b_clock clock;
	fm33256b_clock_read_burst(&clock);
	fm33256b_clock_to_time((time_t *) &time->tv_sec, &clock);
	time->tv_nsec = 0;

}

void clock_set_time(timestamp_t * time) {

	struct fm33256b_clock clock;
	fm33256b_time_to_clock((time_t *) &time->tv_sec, &clock);
	fm33256b_clock_write_burst(&clock);

}

void clock_get_monotonic(timestamp_t * time) {

	uint32_t clocks = xTaskGetTickCount();

	time->tv_sec = clocks / configTICK_RATE_HZ;
	time->tv_nsec = (clocks % configTICK_RATE_HZ) * (1000000000/configTICK_RATE_HZ);

}
