#ifndef LOG_HOST_H_
#define LOG_HOST_H_

#include <stdint.h>
#include <stdarg.h>
#include <util/timestamp.h>
#include <util/vmem.h>

#define LOG_GROUP_MASKED(group, name_in, print_mask_in, store_mask_in) \
	__attribute__((section("log_groups"), used)) \
	log_group_t group##_s = {.name = name_in, .print_mask = print_mask_in, .store_mask = store_mask_in}; \
	log_group_t * group = &group##_s;

#define LOG_GROUP(group, name_in) LOG_GROUP_MASKED(group, name_in, LOG_ERROR_MASK | LOG_WARNING_MASK, LOG_ERROR_MASK | LOG_WARNING_MASK)
#define LOG_GROUP_VERBOSE(group, name_in) LOG_GROUP_MASKED(group, name_in, LOG_ALL_MASK, LOG_ERROR_MASK | LOG_WARNING_MASK)
#define LOG_GROUP_SILENT(group, name_in) LOG_GROUP_MASKED(group, name_in, 0, 0)

#define LOG_GROUP_EXTERN(group) extern log_group_t * group;

static inline int log_check_print_mask(log_group_t * group, uint8_t mask) {
	return ((group->print_mask & mask) > 0);
}

/** Default log group, this can be overridden by a define */
extern log_group_t * LOG_DEFAULT;

/**
 * Serach for a group by its name
 * @param group_name string ptr
 * @return NULL or group ptr
 */
log_group_t * log_find_group(const char * group_name);

void log_set_mask(char * name, uint8_t mask, int is_print_mask);
void log_set_printmask(char * name, uint8_t mask);
void log_set_storemask(char * name, uint8_t mask);

/**
 * Init the log store memory area.
 * This system uses util/vmem as a backend, so please provide a
 * pointer to a memory area, either in RAM or in FRAM using the
 * VMEM system.
 *
 * Note: The vmem_buf must be correctly setup before calling
 * the logging system
 *
 * @param start_addr pointer to RAM or FRAM addr (using VMEM)
 * @param length length of memory area to use this must be >= 512
 */
void log_store_init(vmemptr_t start_addr, int length);

void log_store_foreach(int (*fct)(char * data, unsigned int size));
void log_store(char * data, unsigned int size);

/**
 * Print list of groups to console
 */
void log_print_groups(void);

/**
 * Setup CSP logging
 */
void log_csp_init(void);

/** log_debug
 * Log event with LOG_DEBUG level
 * @param event Event
 * @param format Format string
 * @return 0 if event was entry was successfully sent. -1 if an error occurred
 */
#define log_debug(format, ...) { log_event_group(LOG_DEBUG, LOG_DEFAULT, format, ##__VA_ARGS__); }
#define log_debug_group(group, format, ...) { log_event_group(LOG_DEBUG, group, format, ##__VA_ARGS__); }

/** log_info
 * Log event with LOG_INFO level
 * @param event Event
 * @param format Format string
 * @return 0 if event was entry was successfully sent. -1 if an error occurred
 */
#define log_info(format, ...) { log_event_group(LOG_INFO, LOG_DEFAULT, format, ##__VA_ARGS__); }
#define log_info_group(group, format, ...) { log_event_group(LOG_INFO, group, format, ##__VA_ARGS__); }

/** log_warning
 * Log event with LOG_WARNING level
 * @param event Event
 * @param format Format string
 * @return 0 if event was entry was successfully sent. -1 if an error occurred
 */
#define log_warning(format, ...) { log_event_group(LOG_WARNING, LOG_DEFAULT, format, ##__VA_ARGS__); }
#define log_warning_group(group, format, ...) { log_event_group(LOG_WARNING, group, format, ##__VA_ARGS__); }

/** log_error
 * Log event with LOG_ERROR level
 * @param event Event
 * @param format Format string
 * @return 0 if event was entry was successfully sent. -1 if an error occurred
 */
#define log_error(format, ...) { log_event_group(LOG_ERROR, LOG_DEFAULT, format, ##__VA_ARGS__); }
#define log_error_group(group, format, ...) { log_event_group(LOG_ERROR, group, format, ##__VA_ARGS__); }

/** log_trace
 * Log event with LOG_TRACE level
 * @param event Event
 * @param format Format string
 * @return 0 if event was entry was successfully sent. -1 if an error occurred
 */
#define log_trace(format, ...) { log_event_group(LOG_TRACE, LOG_DEFAULT, format, ##__VA_ARGS__); }
#define log_trace_group(group, format, ...) { log_event_group(LOG_TRACE, group, format, ##__VA_ARGS__); }

/**
 * Generic function for logging events.
 * @note This function should not be called directly from user-space.
 * @param level Level
 * @param event Event
 * @param format Format string
 * @return 0 if the event was entry was successfully sent. -1 if an error occurred
 */
int log_event_group(log_level_t level, log_group_t * group, const char * format, ...)
__attribute__ ((format (__printf__, 3, 4)));

int log_event_group_args(log_level_t level, log_group_t * group, const char * format, va_list args);

#endif /* LOG_HOST_H_ */
