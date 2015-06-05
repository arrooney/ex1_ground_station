/**
 * Default CSP debug hook
 *
 * Copyright 2015 GomSpace ApS. All rights reserved.
 */

#include <log/log.h>

#include <csp/csp.h>

static LOG_GROUP(log_csp, "csp");

void log_csp_debug_hook(csp_debug_level_t level, const char *format, va_list args) {
	log_level_t mapped_level;

	switch (level) {
	/* Regular log levels */
	default:
	case CSP_ERROR:
		mapped_level = LOG_ERROR;
		break;
	case CSP_WARN:
		mapped_level = LOG_WARNING;
		break;
	case CSP_INFO:
		mapped_level = LOG_INFO;
		break;
	/* Extended log levels */
	case CSP_BUFFER:
		mapped_level = LOG_TRACE;
		break;
	case CSP_PACKET:
		mapped_level = LOG_INFO;
		break;
	case CSP_PROTOCOL:
		mapped_level = LOG_DEBUG;
		break;
	case CSP_LOCK:
		mapped_level = LOG_TRACE;
		break;
	}

	/* Pass to log system */
	log_event_group_args(mapped_level, log_csp, format, args);

}

void log_csp_init(void) {
	/* Setup CSP debug hook */
	csp_debug_set_level(CSP_ERROR,    true);
	csp_debug_set_level(CSP_WARN,     true);
	csp_debug_set_level(CSP_INFO,     true);
	csp_debug_set_level(CSP_BUFFER,   true);
	csp_debug_set_level(CSP_PACKET,   true);
	csp_debug_set_level(CSP_PROTOCOL, true);
	csp_debug_set_level(CSP_LOCK,     true);
	csp_debug_hook_set(log_csp_debug_hook);
}
