#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>

/*
 * Function pointers that will be initialized to point to the
 * standard libc implementation of printf and getchar.
 */
static int (*IOHook_Libcvprintf)( const char* format, va_list args ) = NULL;
static int (*IOHook_Libcprintf)( const char* format, ... ) = NULL;
static int (*IOHook_Libcgetchar)() = NULL;

/* 
 * Possible states for IO sources/sinks
 */
enum IOHook_State
  {
    IOHOOK_STATE_STD = 0,	/* State to use standard libc definitions. */
    IOHOOK_STATE_FORTH = 1	/* State to use Forth specific implementations. */
  };

/*
 * IOState handler structure. Used to contain references to
 * all hooks for a particular state.
 */
struct IOHook_Handle
{
  int (*vprintf)( const char*, va_list );
  int (*getchar)( );
};

/*
 * Declaration of handler implementations
 */
static int IOHook_Stdvprintf( const char* format, va_list );
static int IOHook_Stdgetchar( void );
static int IOHook_Forthvprintf( const char* format, va_list );
static int IOHook_Forthgetchar( void );

/*
 * State to handler index
 */
static struct IOHook_Handle IOHook_Index[] =
{
  { .vprintf = IOHook_Stdvprintf, .getchar = IOHook_Stdgetchar },
  { .vprintf = IOHook_Forthvprintf, .getchar = IOHook_Forthgetchar }
};

/* 
 * Current state
 */
static enum IOHook_State IOHook_State = IOHOOK_STATE_STD;

/****************************************************************************************************************/
/* Initialization and overriding of libc standard functions							*/	
/****************************************************************************************************************/
/* 
 * Initialization for hooks. 
 */
int IOHook_Init( )
{
  if( IOHook_Libcvprintf == NULL || IOHook_Libcprintf == NULL || IOHook_Libcgetchar == NULL ) {
    /* Find the standard implementations of these functions. */
    IOHook_Libcvprintf = (int (*)(const char* format, va_list args)) dlsym(RTLD_NEXT, "vprintf");
    IOHook_Libcprintf = (int (*)(const char* format, ...)) dlsym(RTLD_NEXT, "printf");
    IOHook_Libcgetchar = (int (*)()) dlsym(RTLD_NEXT, "getchar");

    /* On failure to find, return false. */
    if( IOHook_Libcvprintf == NULL || IOHook_Libcprintf == NULL || IOHook_Libcgetchar == NULL ) {
      return -1;
    }
  }

  return 1;
}

int printf( const char* format, ... )
{
  va_list args;
  int errCode;
  
  if( !IOHook_Init( ) ) {
    return -1;
  }
  
  va_start(args, format);
  errCode = IOHook_Index[IOHook_State].vprintf(format, args);
  va_end(args);

  return errCode;
}

int getchar( )
{
  if( !IOHook_Init( ) ) {
    return -1;
  }

  return IOHook_Index[IOHook_State].getchar( );
}
  
/****************************************************************************************************************/
/* Hook implementation for standard libc IO.									*/	
/****************************************************************************************************************/
static int IOHook_Stdvprintf( const char* format, va_list args )
{
  IOHook_Libcprintf("Hook: ");
  return IOHook_Libcvprintf(format, args);
}

static int IOHook_Stdgetchar( void )
{
  IOHook_Libcprintf("Hook: ");
  return IOHook_Libcgetchar( );
}

/****************************************************************************************************************/
/* Hook implementation for Forth.										*/	
/****************************************************************************************************************/
static int IOHook_Forthvprintf( const char* format, va_list args )
{
  return -1;
}

static int IOHook_Forthgetchar( void )
{
  return 'n';
}

/****************************************************************************************************************/
/* State change functions.											*/	
/****************************************************************************************************************/
void IOHook_SetForthState( )
{
  IOHook_State = IOHOOK_STATE_FORTH;
};

void IOHook_SetSTDState( )
{
    IOHook_State = IOHOOK_STATE_STD;
}
