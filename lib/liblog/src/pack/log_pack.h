#ifndef LOG_PACK_H_
#define LOG_PACK_H_

#include <stdarg.h>

int log_pack(char * buf, int max_size, const char * format, va_list ap);
va_list * log_unpack(char * buf, int max_size, const char * format, const char * data, int data_len);

#endif /* LOG_PACK_H_ */
