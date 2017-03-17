#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>

extern int printf( const char* format, ... )
{
  static int (*std_vprintf)( const char* format, va_list args ) = NULL;
    static int (*std_printf)( const char* format, ... ) = NULL;
  if( std_vprintf == NULL || std_printf == NULL ) {
    std_vprintf = (int (*)(const char* format, va_list args)) dlsym(RTLD_NEXT, "vprintf");
    std_printf = (int (*)(const char* format, ...)) dlsym(RTLD_NEXT, "printf");
  }

  if( std_vprintf == NULL || std_printf == NULL ) {
    return -1;
  }

  va_list args;
  int errCode;
  
  va_start(args, format);
  std_printf("hook: ");
  errCode = std_vprintf(format, args);
  va_end(args);

  return errCode;
}
