#include <inttypes.h>
#include <stdint.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include <util/clock.h>

void clock_get_time(timestamp_t * time) {

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	time->tv_sec = now.tv_sec;
	time->tv_nsec = now.tv_nsec;

}

void clock_set_time(timestamp_t * time) {

	struct timespec now;
	now.tv_sec = time->tv_sec;
	now.tv_nsec = time->tv_nsec;

	clock_settime(CLOCK_REALTIME, &now);

}

void clock_get_monotonic(timestamp_t * time) {

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	time->tv_sec = now.tv_sec;
	time->tv_nsec = now.tv_nsec;
}
