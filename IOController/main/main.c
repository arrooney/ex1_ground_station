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
#include <unistd.h>
#include <IOHook.h>

void* gomshell_thread( void* arg )
{
	(void) arg;
	char msg;

	for( ;; ) {
		msg = getchar();
		if( msg != '\n' ) {
			printf("\nGomshell got: ");
			printf("%c", msg);
		}
		while( msg != '\n') {
			msg = getchar( );
			printf("%c", msg);
		}
		printf("\n");
	}
	pthread_exit(NULL);
}

void* terminal_input_thread( void* arg )
{
	(void) arg;
	char msg;
	IOHook_Printf_FP printf_fp;
	IOHook_Getchar_FP getchar_fp;
	struct CCThreadedQueue* gomshell_input;

	getchar_fp = IOHook_GetGetchar( );
	printf_fp = IOHook_GetPrintf( );
	gomshell_input = IOHook_GetGomshellInputQueue( );

	printf_fp("Input handler running\n");
	for( ;; ) {
		msg = getchar_fp( );
		CCThreadedQueue_Insert(gomshell_input, &msg, COS_BLOCK_FOREVER);
#ifdef VERIFY
		CCThreadedQueue_Insert(gomshell_input, &msg, COS_BLOCK_FOREVER);
#endif
	}
}

void* terminal_output_thread( void* arg )
{
	(void) arg;
	char msg;
	IOHook_Printf_FP printf_fp;
	struct CCThreadedQueue* gomshell_output;

	printf_fp = IOHook_GetPrintf( );
	gomshell_output = IOHook_GetGomshellOutputQueue( );

	printf_fp("Output handler running\n");
	for( ;; ) {
		CCThreadedQueue_Remove(gomshell_output, &msg, COS_BLOCK_FOREVER);
#ifdef VERIFY
		printf_fp("%c*", msg);
#else
		printf_fp("%c", msg);
#endif
	}
}

int main( int argc, char** argv )
{
	(void) argc; (void) argv;
	pthread_t gomshell_thread_handle;
	pthread_t terminal_input_thread_handle;
	pthread_t terminal_output_thread_handle;
	
	if( IOHook_Init( ) != 0 ) {
		return -1;
	}

	pthread_create(&gomshell_thread_handle, NULL, gomshell_thread, NULL);
	pthread_create(&terminal_input_thread_handle, NULL, terminal_input_thread, NULL);
	pthread_create(&terminal_output_thread_handle, NULL, terminal_output_thread, NULL);
	pthread_join(gomshell_thread_handle, NULL);
	
	return 0;
}
