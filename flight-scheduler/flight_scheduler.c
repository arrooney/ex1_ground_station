/*

This program reads from STDIN in a user-friendly way to build command scripts.

Inputs are: start time of orbit (or script in general) in unix timestamp seconds,
			length of orbit (determined by orbital simulation)
			
and then inputs are:
			time from orbit start in seconds to execute following command,
			command name (just text). ex: "dfgm power","on"

The script is then output in the correct format for reading on the satellite.

Note that the maximum number of commands per file is 250. The length of an orbit is arbitrary.
*/





#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>

#define SCRIPT_MAX_LENGTH (4*20+3)
#define MAX_COMMANDS_PER_SCHEDULE (4*30+3)

struct command_t __ATTRIBUTE__((packed))
{
	uint32_t unix_time;
	uint8_t command_length;
	char command[SCRIPT_MAX_LENGTH];
} command_t;

struct schedule_t __ATTRIBUTE__((packed))
{
	uint8_t total_commands;
	command_t slot[MAX_COMMANDS_PER_SCHEDULE];
};


struct schedule_t bin;
FILE* fid;
char output[SCRIPT_MAX_LENGTH];
char is_exit[1];
int i;
int j;
int count = 0;
int numels = 0;

void main( ) {

	printf("Ex-Alta 1 Flight Schedule Generator V1.0\n");

	for( i = 0; i < MAX_COMMANDS_PER_SCHEDULE; i++ ) {
		printf("This is slot %d of %d command slots\n\n", i, MAX_COMMANDS_PER_SCHEDULE);

		/* Read in unix execution time.
		 */
		printf("Enter unix time of command execution:\n");
		printf("Enter a value : ");
		scanf("%hu", &bin.slot[i].unix_time);
		printf("Scheduling command for  %" PRIu32 "\n", bin.slot[i].unix_time);
		bin.slot[i].unix_time = htole32(bin.slot[i].unix_time);

		/* Read in command to execute
		 */
		printf("Enter the command to execute using the format: \"command\", \"arg\"\n");
		scanf(" %99[^\n]", bin.slot[i].command);
		int copied = snprintf(output, SCRIPT_MAX_LENGTH, "COMMAND(%s);\n", bin.slot[i].command);
		if( copied >= SCRIPT_MAX_LENGTH ) {
			printf("Command string too large, exiting scheduler..\n");
			return -1;
		}
		memcpy(bin.slot[i].command, output, strlen(output));
		bin.slot[i].command_length = strlen(bin.slot[i].command);

		/* Update total commands in schedule.
		 */
		bin.total_commands = htole32((i+1));

		/* Exit if user has no more commands to schedule.
		 */
		printf("Done? Press y to exit here, n to continue\n");
		scanf("%c", is_exit);
		if( strcmp(is_exit,"y") == 0 ) {
			break;
		}
	}

	/* Write the schedule into a file for upload to S/C.
	 */
	printf("Compiling flight schedule containing %d commands:\n", le32toh(bin.total_commands));
	fid = fopen("script.bin","w");
	if( fid == NULL ) {
		prinf("Failed to create file for compiled schedule\n");
		return -1;
	}
	size_t written = fwrite(&bin, sizeof(bin), 1, fid);
	if( written != 1 ) {
		printf("Failed to write compiled schedule to file\n");
		return -1;
	}
	fclose(fid);
	printf("Done!\n");
	return 0;
}
