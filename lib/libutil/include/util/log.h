/**
 * Generic log interface with support for:
 * 	LOG_DISABLED
 * 	LOG_PRINTF
 * 	LOG_COLOR
 * 	LOG_CDH
 *
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <conf_util.h>

#ifdef __AVR__
#undef printf
#undef sscanf
#undef scanf
#undef sprintf
#undef snprintf
#undef vprintf
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#define printf(s, ...) printf_P(PSTR(s), ## __VA_ARGS__)
#define sscanf(buf, s, ...) sscanf_P(buf, PSTR(s), ## __VA_ARGS__)
#define scanf(s, ...) scanf_P(PSTR(s), ## __VA_ARGS__)
#define sprintf(buf, s, ...) sprintf_P(buf, PSTR(s), ## __VA_ARGS__)
#define snprintf(buf, size, s, ...) snprintf_P(buf, size, PSTR(s), ## __VA_ARGS__)
#define COLOR_PRINTF(color, format, ...) color_printf(color, PSTR(format), ## __VA_ARGS__)
#define vprintf(format, args) vfprintf_P(stdout, format, args)
#else
#include <stdio.h>
#define COLOR_PRINTF(color, format, ...) color_printf(color, format, ## __VA_ARGS__)
#endif

#if defined(LOG_DISABLE)

#define log_count(...) do {} while(0);
#define log_debug(...) do {} while(0);
#define log_info(...) do {} while(0);
#define log_warning(...) do {} while(0);
#define log_error(...) do {} while(0);
#define log_debug_group(...) do {} while(0);
#define log_info_group(...) do {} while(0);
#define log_warning_group(...) do {} while(0);
#define log_error_group(...) do {} while(0);
#define LOG_GROUP(...)
#define LOG_GROUP_MASKED(...)
#define LOG_GROUP_VERBOSE(...)
#define LOG_GROUP_SILENT(...)

#elif defined(LOG_PRINTF)
#define log_debug(format, ...) { 	do { if (format != NULL) {printf(format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_info(format, ...) { 	do { if (format != NULL) {printf(format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_warning(format, ...) { 	do { if (format != NULL) {printf(format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_error(format, ...) { 	do { if (format != NULL) {printf(format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }

#elif defined(LOG_COLOR)

#include <util/color_printf.h>
#define log_debug(format, ...) { 	do { if (format != NULL) { COLOR_PRINTF(COLOR_BLUE, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_info(format, ...) { 		do { if (format != NULL) { COLOR_PRINTF(COLOR_GREEN, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_warning(format, ...) { 	do { if (format != NULL) { COLOR_PRINTF(COLOR_YELLOW, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_error(format, ...) { 	do { if (format != NULL) { COLOR_PRINTF(COLOR_RED, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_debug_group(grp, format, ...) { 	do { if (format != NULL) { COLOR_PRINTF(COLOR_BLUE, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_info_group(grp, format, ...) { 		do { if (format != NULL) { COLOR_PRINTF(COLOR_GREEN, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_warning_group(grp, format, ...) { 	do { if (format != NULL) { COLOR_PRINTF(COLOR_YELLOW, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define log_error_group(grp, format, ...) { 	do { if (format != NULL) { COLOR_PRINTF(COLOR_RED, format, ##__VA_ARGS__); } printf("\r\n"); } while(0); }
#define LOG_GROUP(...)
#define LOG_GROUP_MASKED(...)
#define LOG_GROUP_VERBOSE(...)
#define LOG_GROUP_SILENT(...)

#elif defined(LOG_CDH)
#include <log/log.h>
#endif
