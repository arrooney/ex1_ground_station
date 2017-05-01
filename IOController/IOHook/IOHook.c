/*
 * Copyright 2017 Brendan Bruner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * bbruner@ualberta.ca
 * April 11, 2017
 */
/**
 * @file
 * @defgroup IOHook
 * Redefinition of standard io functions like printf() and getchar().
 */
#include <IOHook.h>
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <pthread.h>
#include <CCThreadedQueue.h>
#include <CCArrayQueue.h>

#define IOHOOK_STRING_BUFFER_LENGTH 1024
#define GOMSHELL_IO_ELEMENT_SIZE sizeof(char)
#define GOMSHELL_IO_BUFFER_SIZE 1024

/* Function pointers that will be initialized to point to the
 * standard libc implementation of printf and getchar. ie,
 * these are the functions that must be called to do IO with
 * the terminal.
 */
static IOHook_Printf_FP IOHook_Libcprintf = NULL;
static IOHook_Getchar_FP IOHook_Libcgetchar = NULL;

/* Variables used for buffering data
 */
static char iohook_string_buffer[IOHOOK_STRING_BUFFER_LENGTH];
static struct CCThreadedQueue gomshell_output;
static struct CCArrayQueue gomshell_output_backbone;
static struct CCThreadedQueue gomshell_input;
static struct CCArrayQueue gomshell_input_backbone;


/****************************************************************************************************************/
/* Function definitions.											*/
/****************************************************************************************************************/
/**
 * @ingroup IOHook
 * @brief
 * 	Initialization for hooks. 
 * @return 
 *	true/false if initialization was successful/failed
 */
int IOHook_Init( )
{
	CError err;
	
	if( IOHook_Libcprintf == NULL || IOHook_Libcgetchar == NULL ) {
		/* Find the standard implementations printf, vprintf, and getchar. Note that the
		 * type cast will give a compile time warning. It says an object pointer should not
		 * be converted to function pointer. While generally one should never do this,
		 * there is no other way to reference the std implementation of printf, vprintf
		 * and getchar.
		 */
		IOHook_Libcprintf = (int (*)(const char* format, ...)) dlsym(RTLD_NEXT, "printf");
		IOHook_Libcgetchar = (int (*)()) dlsym(RTLD_NEXT, "getchar");

		/* On failure to find, return false. 
		 */
		if( IOHook_Libcprintf == NULL || IOHook_Libcgetchar == NULL ) {
			return -1;
		}
	}

	err = CCArrayQueue(&gomshell_input_backbone, GOMSHELL_IO_ELEMENT_SIZE, GOMSHELL_IO_BUFFER_SIZE);
	if( err != COBJ_OK ) {
		return -1;
	}
	err = CCThreadedQueue(&gomshell_input, &gomshell_input_backbone.ciqueue);
	if( err != COBJ_OK ) {
		CDestroy(&gomshell_input_backbone);
		return -1;
	}

	err = CCArrayQueue(&gomshell_output_backbone, GOMSHELL_IO_ELEMENT_SIZE, GOMSHELL_IO_BUFFER_SIZE);
	if( err != COBJ_OK ) {
		CDestroy(&gomshell_input);
		return -1;
	}
	err = CCThreadedQueue(&gomshell_output, &gomshell_output_backbone.ciqueue);
	if( err != COBJ_OK ) {
		CDestroy(&gomshell_input);
		CDestroy(&gomshell_output_backbone);
		return -1;
	}
	
	
	return 0;
}

/**
 * @ingroup IOHook
 * @brief
 *	Override of standard printf() function
 * @details
 * 	All data is written to the software serial device provided as input to the
 * 	IOHook_Init() funciton. The device acts a slave on the serial bus, so no
 * 	data gets written to the bus if the device is selected by a bus master.
 */
static int goawaycompiler_vprintf( const char* format, va_list args )
{
	int bytes_copied, i;
	CCTQueueError err;
	
	bytes_copied = vsnprintf(iohook_string_buffer, IOHOOK_STRING_BUFFER_LENGTH, format, args);

	for( i = 0; i < bytes_copied; ++i ) {
		err = CCThreadedQueue_Insert(&gomshell_output, &iohook_string_buffer[i], COS_BLOCK_FOREVER);
		if( err != CCTQUEUE_OK ) {
			break;
		}
	}
	
	return i;
}

static int goawaycompiler_printf( const char* format, ... )
{
	va_list args;
	int bytes;

	va_start(args, format);
	bytes = goawaycompiler_vprintf(format, args);
	va_end(args);
	return bytes;
}

int printf( const char* format, ... )
{
	va_list args;
	int bytes;

	va_start(args, format);
	bytes = goawaycompiler_vprintf(format, args);
	va_end(args);
	return bytes;
}

int puts( const char* str )
{
	int bytes;
	bytes = goawaycompiler_printf(str);
	goawaycompiler_printf("\n");
	return bytes;
}

int putchar( int byte )
{
	goawaycompiler_printf("%c", (char) byte);
	return byte;
}

/**
 * @ingroup IOHook
 * @brief
 *	Override of standard getchar() function
 * @details
 * 	Data is read from the software serial device provided as input to the
 * 	IOHook_Init() function. The device acts a slave on the serial bus, so no
 * 	data gets read from the bus until the device is selected by a bus master.
 *	This function does a blocking wait for data.
 */
int getchar( )
{
	char msg;
	CCTQueueError err;

	/* Block until given access to the software serial bus.
	 * try to read from the bus. If for some reason an error
	 * occurs reading from the bus, try again.
	 */
	do {
		err = CCThreadedQueue_Remove(&gomshell_input, &msg, COS_BLOCK_FOREVER);
	} while( err != CCTQUEUE_OK );

	return (int) msg;
}

char* gets( char* str )
{
	char msg;
	CCTQueueError err;
	int i;

	i = 0;
	do {
		err = CCThreadedQueue_Remove(&gomshell_input, &msg, COS_BLOCK_FOREVER);
		str[i] = msg;
		++i;
	} while( err == CCTQUEUE_OK && msg != '\n' );
	str[i-1] = '\0';
	return str;
}
	

/**
 * @ingroup IOHook
 * @brief
 * 	Returns a function pointer to the standard implementation of printf().
 */
IOHook_Printf_FP IOHook_GetPrintf( )
{
	return IOHook_Libcprintf;
}

/**
 * @ingroup IOHook
 * @brief
 * 	Returns a function pointer to the standard implementation of getchar().
 */
IOHook_Getchar_FP IOHook_GetGetchar( )
{
	return IOHook_Libcgetchar;
}

/**
 * @ingroup IOHook
 * @brief
 *	Get a reference to the queue where all output from the gomshell is put.
 */
struct CCThreadedQueue* IOHook_GetGomshellOutputQueue( )
{
	return &gomshell_output;
}

/**
 * @ingroup IOHook
 * @brief
 *	Get a reference to the queue where the gomshell pulls all its input from.
 */
struct CCThreadedQueue* IOHook_GetGomshellInputQueue( )
{
	return &gomshell_input;
}
