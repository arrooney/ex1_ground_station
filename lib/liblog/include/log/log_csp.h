#ifndef LOG_CSP_H_
#define LOG_CSP_H_

#include <csp/csp.h>

void log_csp_debug_hook(csp_debug_level_t level, const char *format, va_list args);

#endif /* LOG_CSP_H_ */
