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
#include <string.h>
#include <stdio.h>
#include <unit.h>
#include <CCSoftSerialDev.h>
#include <unistd.h>
#include <CCSoftSerialBus.h>
#include <IOHook.h>
#include <semaphore.h>

#define IO_BUS_CHANNEL_LENGTH 5096
#define IO_MASTER_PRIO 2
#define IO_FORTH_PRIO 1

static struct CCSoftSerialDev io_slave;
struct CCSoftSerialDev io_master;
struct CCSoftSerialBus io_bus;
#ifndef UNIT_TEST_SUITE
struct CCSoftSerialDev io_forth;
sem_t terminal_input_arbitration;
#endif /* UNIT_TEST_SUITE */



#ifdef UNIT_TEST_SUITE

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

#else

void* gomshell_thread( void* arg )
{
	(void) arg;
	char msg;

	for( ;; ) {
		msg = getchar( );
		if( msg != '\n' ) {
			printf("Gomshell got: ");
			printf("%c", msg);
		}
		while( msg != '\n') {
			msg = getchar( );
			printf("%c", msg);
		}
	}
	pthread_exit(NULL);
}

void* terminal_input_thread( void* arg )
{
	(void) arg;
	char msg;
	IOHook_Getchar_FP getchar_fp;
	IOHook_Printf_FP printf_fp;
	CCSoftSerialError err;

	printf_fp = IOHook_GetPrintf( );
	getchar_fp = IOHook_GetGetchar( );
	for( ;; ) {
		msg = getchar_fp( );
		CCSoftSerialDev_Select(&io_master, IO_SLAVE_ID, COS_BLOCK_FOREVER);
		CCSoftSerialDev_Write(&io_master, &msg, COS_BLOCK_FOREVER);
		while( msg != '\n' ) {
			msg = getchar_fp( );
			CCSoftSerialDev_Write(&io_master, &msg, COS_BLOCK_FOREVER);
		}
		do {
			err = CCSoftSerialDev_Read(&io_master, &msg, COS_BLOCK_FOREVER);
			if( err == CCSOFTSERIAL_OK ) {
				printf_fp("%c", msg);
			}
			else {
				printf_fp("Bus Error\n");
				break;
			}
		} while( msg != '\n' );
		CCSoftSerialDev_Unselect(&io_master);
	}
	pthread_exit(NULL);
}

void* forth_input_thread( void* arg )	
{
	(void) arg;
	IOHook_Printf_FP printf_fp;
	char msg;
	CCSoftSerialError err;
	const char* forth_msg = "Forth kernal command\n";
	size_t forth_msg_len, i;

	printf_fp = IOHook_GetPrintf( );
	forth_msg_len = strlen(forth_msg);
	for( ;; ) {
		CCSoftSerialDev_Select(&io_forth, IO_SLAVE_ID, COS_BLOCK_FOREVER);
		for( i = 0; i < forth_msg_len; ++i ) {
			CCSoftSerialDev_Write(&io_forth, &forth_msg[i], COS_BLOCK_FOREVER);
		}
		do {
			err = CCSoftSerialDev_Read(&io_forth, &msg, COS_BLOCK_FOREVER);
			if( err == CCSOFTSERIAL_OK ) {
				printf_fp("%c", msg);
			}
			else {
				printf_fp("Bus Error\n");
				break;
			}
		} while( msg != '\n' );
		CCSoftSerialDev_Unselect(&io_forth);
		sleep(1);
	}
	pthread_exit(NULL);
}

#endif /* UNIT_TEST_SUITE */

int main( int argc, char** argv )
{
	(void) argc; (void) argv;
	#ifdef UNIT_TEST_SUITE
	pthread_t thread;
	#else
	pthread_t gomshell_thread_handle;
	pthread_t terminal_input_thread_handle;
	pthread_t forth_input_thread_handle;
	#endif /* UNIT_TEST_SUITE */

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
	#ifndef UNIT_TEST_SUITE
	if( CCSoftSerialDevMaster(&io_forth, IO_FORTH_PRIO, IO_FORTH_ID, &io_bus) != COBJ_OK ) {
		return -1;
	}
	#endif /* UNIT_TEST_SUITE */
	if( IOHook_Init(&io_slave) != CTRUE ) {
		return -1;
	}

	#ifdef UNIT_TEST_SUITE
	pthread_create(&thread, NULL, print_thread, NULL);
	RUN_TEST_SUITE(iohook);
	PRINT_DIAG( );
	pthread_join(thread, NULL);
	#else
	pthread_create(&gomshell_thread_handle, NULL, gomshell_thread, NULL);
	pthread_create(&terminal_input_thread_handle, NULL, terminal_input_thread, NULL);
	pthread_create(&forth_input_thread_handle, NULL, forth_input_thread, NULL);
	pthread_join(gomshell_thread_handle, NULL);
	pthread_join(terminal_input_thread_handle, NULL);
	pthread_join(forth_input_thread_handle, NULL);
	#endif /* UNIT_TEST_SUITE */

	return 0;
}
