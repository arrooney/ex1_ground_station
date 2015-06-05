#ifndef LOG_TYPES_H_
#define LOG_TYPES_H_

#include <stdint.h>

typedef enum __attribute__ ((packed)) {
	LOG_TRACE = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	/* This must be the last entry */
	LOG_LEVEL_MAX,
} log_level_t;

typedef struct log_group {
	char * name;
	uint8_t print_mask;
	uint8_t store_mask;
} log_group_t;

typedef struct __attribute__((packed)) {
	uint8_t level;
	log_group_t * group;
	const char * format;
	uint32_t sec;
	uint16_t msec;
	uint8_t len;
	char data[];
} log_store_t;

#define LOG_TRACE_MASK		(1 << LOG_TRACE)
#define LOG_DEBUG_MASK		(1 << LOG_DEBUG)
#define LOG_INFO_MASK		(1 << LOG_INFO)
#define LOG_WARNING_MASK	(1 << LOG_WARNING)
#define LOG_ERROR_MASK		(1 << LOG_ERROR)
#define LOG_ALL_MASK		((1 << LOG_LEVEL_MAX) - 1)

#define LOG_DEFAULT_MASK	(LOG_ERROR_MASK | LOG_WARNING_MASK)

#endif /* LOG_TYPES_H_ */
