/**
 * Satellite logging client
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2010 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <alloca.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <util/timestamp.h>
#include <util/byteorder.h>
#include <util/clock.h>
#include <util/color_printf.h>
#include <uthash/uthash.h>

#include <log/log.h>

#include "../pack/log_pack.h"
#include "log_level_count.h"

extern log_group_t __start_log_groups;
extern log_group_t __stop_log_groups;

LOG_GROUP_VERBOSE(log_default, "default");
log_group_t * LOG_DEFAULT = &log_default_s;

log_group_t * log_find_group(const char * group_name) {
	log_group_t * found_group = NULL;
	for (log_group_t * group = &__start_log_groups; group != (log_group_t *) &__stop_log_groups; group++) {
		if (strcmp(group->name, group_name) == 0) {
			found_group = group;
		}
	}
	return found_group;
}

void log_print_groups(void) {
	printf("Group      Print Store\r\n");
	for (log_group_t * group = &__start_log_groups; group != (log_group_t *) &__stop_log_groups; group++) {

		printf("%-10s %c%c%c%c%c %c%c%c%c%c\r\n", group->name,
		       /* Print masks */
		       (group->print_mask & LOG_ERROR_MASK) ? 'E' : '.',
		       (group->print_mask & LOG_WARNING_MASK)  ? 'W' : '.',
		       (group->print_mask & LOG_INFO_MASK) ? 'I' : '.',
		       (group->print_mask & LOG_DEBUG_MASK) ? 'D' : '.',
		       (group->print_mask & LOG_TRACE_MASK) ? 'T' : '.',
		       /* Store masks */
		       (group->store_mask & LOG_ERROR_MASK) ? 'E' : '.',
		       (group->store_mask & LOG_WARNING_MASK) ? 'W' : '.',
		       (group->store_mask & LOG_INFO_MASK) ? 'I' : '.',
		       (group->store_mask & LOG_DEBUG_MASK) ? 'D' : '.',
		       (group->store_mask & LOG_TRACE_MASK) ? 'T' : '.');
	}
}

void log_set_mask(char * name, uint8_t mask, int is_print_mask) {

	/* For all */
	if (strcmp(name, "all") == 0) {
		for (log_group_t * group = &__start_log_groups; group != (log_group_t *) &__stop_log_groups; group++) {
			if (is_print_mask) {
				group->print_mask = mask;
			} else {
				group->store_mask = mask;
			}
		}
		return;
	}

	log_group_t * group = log_find_group(name);
	if (group == NULL)
		return;

	if (is_print_mask) {
		group->print_mask = mask;
	} else {
		group->store_mask = mask;
	}

}

void log_set_printmask(char * name, uint8_t mask) {
	log_set_mask(name, mask, 1);
}

void log_set_storemask(char * name, uint8_t mask) {
	log_set_mask(name, mask, 0);
}

int log_event_group_args(log_level_t level, log_group_t * group, const char * format, va_list args) {

	va_list store_args;

	if (group == NULL)
		group = LOG_DEFAULT;

	int do_print = ((group->print_mask & (1 << level)) > 0);
	int do_store = ((group->store_mask & (1 << level)) > 0);

	if (do_print == 0 && do_store == 0)
		return -1;

	/* Get time as close as possible to the event */
	timestamp_t ts;
	clock_get_monotonic(&ts);

	if (do_store)
		va_copy(store_args, args);

	/**
	 * Print out nicely
	 */
	if (do_print) {

		/* Start color */
		switch (level) {
			default:
			case LOG_TRACE: printf("\E[0;35m"); break;	/* Magenta */
			case LOG_DEBUG: printf("\E[0;34m"); break;	/* Blue */
			case LOG_INFO: printf("\E[0;32m"); break;	/* Green */
			case LOG_WARNING: printf("\E[0;33m"); break;	/* Yellow */
			case LOG_ERROR: printf("\E[1;31m"); break;	/* Red */
		}

		/* Print time and group name */
		printf("%04"PRIu32".%03"PRIu32" %s: ", ts.tv_sec, ts.tv_nsec / 1000000, group->name);

		/* Print log message */
		vprintf(format, args);

		/* End color */
		printf("\E[0m\r\n");

	}

	/**
	 * Increment counters
	 */
	log_level_count(level);

	/**
	 * Store in a nice compact format
	 */
	if (do_store) {

		/* Try to pack data on stack */
		log_store_t * store = alloca(100);

		store->level = level;
		store->group = group;
		store->format = format;
		store->sec = ts.tv_sec;
		store->msec = ts.tv_nsec / 1000000;

		store->len = log_pack(store->data, 80, format, store_args);

		log_store((void *) store, store->len + offsetof(log_store_t, data));

	}

	return 0;

}

int log_event_group(log_level_t level, log_group_t * group, const char * format, ...) {
	va_list va_args;
	va_start(va_args, format);
	int result = log_event_group_args(level, group, format, va_args);
	va_end(va_args);
	return result;
}
