#include "pfunix.h"
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void processBlock( cell_t wake_time_ )
{
	uint32_t wake_time;
	uint32_t current_time;
	uint32_t time_to_block;
	
	wake_time = (uint32_t) wake_time_;
	current_time = time(NULL);
	
	if( current_time > wake_time ) {
		return;
	}
	
	time_to_block = wake_time - current_time;
	sleep(time_to_block);
}

void unixShellCommand( cell_t cmd_string_, cell_t length_ )
{
	char* cmd_string;
	char* null_term_cmd_string;
	size_t length;

	cmd_string = (char*) cmd_string_;
	length = (size_t) length_;

	null_term_cmd_string = malloc(sizeof(char)*(length+1));
	if( null_term_cmd_string == NULL ) {
		return;
	}

	strncpy(null_term_cmd_string, cmd_string, length);
	null_term_cmd_string[length] = '\0';

	system(null_term_cmd_string);
}

void processSleep( cell_t sleep_time_ )
{
	uint32_t sleep_time;

	sleep_time = (uint32_t) sleep_time_;
	sleep(sleep_time);
}

void programExit( void )
{
	exit(EXIT_SUCCESS);
}


