#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include <log/log_types.h>

/** Level and event counters */
static uint32_t log_level_counter[LOG_LEVEL_MAX];

void log_level_count(log_level_t level) {
	if (level > LOG_LEVEL_MAX)
		return;
	log_level_counter[level]++;
	/** Todo: store to FRAM */
}

void log_level_count_init(void) {
	/** Todo: read count from FRAM */
}

void log_level_count_reset(void) {
	memset(&log_level_counter, 0, sizeof(log_level_counter));
	/** Todo: store to FRAM */
}
