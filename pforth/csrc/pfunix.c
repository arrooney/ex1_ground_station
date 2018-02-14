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

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define LGREEN   "\x1b[32;1m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"
#define GREY	   "\x1b[30;1m"
void colorType( cell_t addr_, cell_t len_, cell_t color_ )
{
	char* str = (char*) addr_;
	size_t len = (size_t) len_;
	int color = (int) color_;
	
	switch( color ) {
	case 1:
		/* Red */
		sdTerminalPrint(RED "%.*s" C_RESET, len, str);
		break;
	case 2:
		/* Yellow */
		sdTerminalPrint(YELLOW "%.*s" C_RESET, len, str);
		break;
	case 3:
		/* Green */
		sdTerminalPrint(GREEN "%.*s" C_RESET, len, str);
		break;
	case 4:
		/* Blue */
		sdTerminalPrint(CYAN "%.*s" C_RESET, len, str);
		break;
	case 5:
		/* Magenta */
		sdTerminalPrint(MAGENTA "%.*s" C_RESET, len, str);
		break;
	case 6:
		/* Grey */
		sdTerminalPrint(GREY "%.*s" C_RESET, len, str);
		break;
	case 7:
	default:
		/* Reset */
		sdTerminalPrint(C_RESET "%.*s", len, str);
		break;
	}
}

void timeStamp( void )
{
	char time_string[32];
	snprintf(time_string, 32, "%d ", time(NULL));
	sdTerminalPrint(time_string, strlen(time_string));
}

void pushUnixTime( void )
{
	PUSH_DATA_STACK((cell_t) time(NULL));
}
