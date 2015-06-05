#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <log/log.h>

int log_pack(char * buf, int max_size, const char * format, va_list ap) {
	return vsnprintf(buf, max_size, format, ap);
}
