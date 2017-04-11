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
 */
#include <stdio.h>
#ifndef UNIT_TEST_SUITE

void* gomshell_thread( void* arg )
{
	char msg;

	for( ;; ) {
		msg = getchar( );
		if( msg != '\n' ) {
			printf("Gomshell got:");
		}
		while( msg != '\n') {
			msg = getchar( );
			printf("%c", msg);
		}
	}
}

void* terminal_thread( void* arg )
{
	(void) arg;
	char msg;
	CCSoftSerialError err;
	IOHook_Printf_FP printf_fp;
	IOHook_Getchar_FP getchar_fp;

	printf_fp = IOHook_GetPrintf( );
	getchar_fp = IOHook_GetGetchar( );

	for( ;; ) {
		msg = getchar_fp( );
		CCSoftSerialDev_Select(&io_terminal_master, IO_SLAVE_ID, COS_BLOCK_FOREVER);
		CCSoftSerialDev_Write(&io_terminal_master, &msg, COS_BLOCK_FOREVER);
		while( msg != '\n' ) {
			msg = getchar( );
			CCSoftSerialDev_Write(&io_terminal_master, &msg, COS_BLOCK_FOREVER);
		}
		do {
			err = CCSoftSerialDev_Read(&io_terminal_master, &msg, COS_NO_BLOCK);
			if( err == CCSOFTSERIAL_OK ) {
				printf_fp("%c", msg);
			}
		} while( msg != '\n' );
		CCSoftSerialDev_Unselect(&io_terminal_master);
	}
	pthread_exit(NULL);
}

void* forth_thread( void* arg )
{
}

int main( int argc, char** argv )
{
  return 0;
}
    
#else
#include <unit.h>
#include <CCSoftSerialDev.h>
#include <CCSoftSerialBus.h>
#include <IOHook.h>

#define IO_BUS_CHANNEL_LENGTH 5096
#define IO_MASTER_PRIO 1

static struct CCSoftSerialDev io_slave;
struct CCSoftSerialDev io_master;
struct CCSoftSerialBus io_bus;

extern TEST_SUITE(iohook);

void* print_thread( void* arg )
{
	(void) arg;
	char msg;
	CCSoftSerialError err;
	IOHook_Printf_FP printf_fp;

	printf_fp = IOHook_GetPrintf( );
	CCSoftSerialDev_Select(&io_master, IO_SLAVE_ID, COS_BLOCK_FOREVER);
	for( ;; ) {
		err = CCSoftSerialDev_Read(&io_master, &msg, COS_BLOCK_FOREVER);
		if( err == CCSOFTSERIAL_OK ) {
			printf_fp("%c", msg);
		}
		else {
			printf_fp("print err\n");
		}
	}

	pthread_exit(NULL);
}


int main( int argc, char** argv )
{
	(void) argc; (void) argv;
	pthread_t thread;

	/* Initialize IOHook. 
	 */
	if( CCSoftSerialBus(&io_bus, sizeof(char), IO_BUS_CHANNEL_LENGTH) != COBJ_OK ) {
		return -1;
	}
	if( CCSoftSerialDevSlave(&io_slave, IO_SLAVE_ID, &io_bus) != COBJ_OK ) {
		return -1;
	}
	if( CCSoftSerialDevMaster(&io_master, IO_MASTER_PRIO, IO_MASTER_ID, &io_bus) != COBJ_OK ) {
		return -1;
	}
	if( IOHook_Init(&io_slave) != CTRUE ) {
		return -1;
	}

	pthread_create(&thread, NULL, print_thread, NULL);
	RUN_TEST_SUITE(iohook);
	PRINT_DIAG( );

	pthread_join(thread, NULL);
	return 0;
}


#endif
