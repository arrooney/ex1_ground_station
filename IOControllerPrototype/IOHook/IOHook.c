#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>

/*
 * Function pointers that will be initialized to point to the
 * standard libc implementation of printf and getchar. ie,
 * these are the functions that must be called to do IO with
 * the terminal.
 */
static int (*IOHook_Libcvprintf)( const char* format, va_list args ) = NULL;
static int (*IOHook_Libcprintf)( const char* format, ... ) = NULL;
static int (*IOHook_Libcgetchar)() = NULL;

/* 
 * Possible states of IO sources/sinks
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
 * Declaration of functions. See function definition for a description.
 */
static int IOHook_Stdvprintf( const char* format, va_list );
static int IOHook_Stdgetchar( void );
static int IOHook_Forthvprintf( const char* format, va_list );
static int IOHook_Forthgetchar( void );

/*
 * The current state, IOHook_State (defined below), is used
 * to index this array. If the current state is IOHOOK_STATE_STD,
 * then the std versions of printf/getchar are called when ever application code
 * makes a call to printf() or getchar().
 * If the current state is IOHOOK_STATE_FORTH, then the forth versions
 * of printf/getchar will be called when ever application code
 * makes a call to printf() or getchar().
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
		/* Find the standard implementations printf, vprintf, and getchar. Note that the
		 * type cast will give a compile time warning. It says an object pointer should not
		 * be converted to function pointer. While generally one should never do this,
		 * there is no other way to reference the std implementation of printf, vprintf
		 * and getchar.
		 */
		IOHook_Libcvprintf = (int (*)(const char* format, va_list args)) dlsym(RTLD_NEXT, "vprintf");
		IOHook_Libcprintf = (int (*)(const char* format, ...)) dlsym(RTLD_NEXT, "printf");
		IOHook_Libcgetchar = (int (*)()) dlsym(RTLD_NEXT, "getchar");

		/* On failure to find, return false. 
		 */
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
/* These are the printf/getchar functions that will be used when in the IOHOOK_STATE_STD
 */
static int IOHook_Stdvprintf( const char* format, va_list args )
{
	return IOHook_Libcvprintf(format, args);
}

static int IOHook_Stdgetchar( void )
{
	return IOHook_Libcgetchar( );
}

/****************************************************************************************************************/
/* Hook implementation for Forth.										*/	
/****************************************************************************************************************/
/* These are the printf/getchar functions that will be used when in the IOHOOK_STATE_FORTH
 */
static int IOHook_Forthvprintf( const char* format, va_list args )
{
	(void) format;
	(void) args;
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
}

void IOHook_SetSTDState( )
{
	IOHook_State = IOHOOK_STATE_STD;
}
