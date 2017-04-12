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

#define IOHOOK_STRING_BUFFER_LENGTH 1024

/* Function pointers that will be initialized to point to the
 * standard libc implementation of printf and getchar. ie,
 * these are the functions that must be called to do IO with
 * the terminal.
 */
static IOHook_Printf_FP IOHook_Libcprintf = NULL;
static IOHook_Getchar_FP IOHook_Libcgetchar = NULL;

/* Variables used for buffering data
 */
static struct CCSoftSerialDev* iohook_device = NULL;
static char iohook_string_buffer[IOHOOK_STRING_BUFFER_LENGTH];
static pthread_mutex_t print_lock;

/****************************************************************************************************************/
/* Function definitions.											*/
/****************************************************************************************************************/
/**
 * @ingroup IOHook
 * @brief
 * 	Initialization for hooks. 
 * @param device
 *	An intialized software serial object with a bus attached to it. See documentation/source code
 * 	for details on what the software serial class is for.
 * @return 
 *	true/false if initialization was successful/failed
 */
CBool IOHook_Init( struct CCSoftSerialDev* device )
{
	if( device == NULL ) {
		return CFALSE;
	}

	if( pthread_mutex_init(&print_lock, NULL) != 0 ) {
		return CFALSE;
	}

	iohook_device = device;
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
			return CFALSE;
		}
	}
	return CTRUE;
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
	CCSoftSerialError err;
	
	bytes_copied = vsnprintf(iohook_string_buffer, IOHOOK_STRING_BUFFER_LENGTH, format, args);

	if( CCSoftSerialDev_Isselected(iohook_device, COS_BLOCK_FOREVER) != CCSOFTSERIAL_OK ) {
		/* Device doesn't have a  channel to write to.
		 */
		IOHook_Libcprintf("printf: no channel\n");
		return 0;
	}

	/* Write the message into the software serial bus.
	 */
	for( i = 0; i < bytes_copied; ++i ) {
		err = CCSoftSerialDev_Write(iohook_device, &iohook_string_buffer[i], COS_BLOCK_FOREVER);
		if( err != CCSOFTSERIAL_OK ) {
			/* Lost access to the bus, abort write.
			 */
			IOHook_Libcprintf("printf: lost channel\n");
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
	CCSoftSerialError err;

	/* Block until given access to the software serial bus.
	 * try to read from the bus. If for some reason an error
	 * occurs reading from the bus, try again.
	 */
	do {
		CCSoftSerialDev_Isselected(iohook_device, COS_BLOCK_FOREVER);
		err = CCSoftSerialDev_Read(iohook_device, &msg, COS_BLOCK_FOREVER);
	} while( err != CCSOFTSERIAL_OK );

	return (int) msg;
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
