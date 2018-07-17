/*
 * schedule.c
 *
 *  
 *      Author: andrew
 *     
 */
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
#include <inttypes.h>
#include <endian.h>

#define SCRIPT_MAX_LENGTH (4*20+3)
#define MAX_COMMANDS_PER_SCHEDULE (4*60+3)

struct command_t
{
	uint32_t unix_time;
	uint8_t command_length;
	char command[SCRIPT_MAX_LENGTH];
} __attribute__((packed));

struct schedule_t
{
	uint8_t total_commands;
	struct command_t slot[MAX_COMMANDS_PER_SCHEDULE];
} __attribute__((packed));


struct schedule_t bin;
FILE* fid;
char output[SCRIPT_MAX_LENGTH];
char is_exit;
int i;
int j;
int count = 0;
int numels = 0;


/* Changed what was previously in the main to the following function to be called in a loop...*/

// Parameters: the index for the sceduled event user wants to overwrite....
void append_schedule(int index){
	int i = index;

	printf("This is slot %d of %d command slots\n", i, MAX_COMMANDS_PER_SCHEDULE);

	/* Read in unix execution time.
	*/
	printf("Enter unix time of command execution:\n");
	printf("Enter a value : ");
	scanf("%" SCNu32, &bin.slot[i].unix_time);
	bin.slot[i].unix_time = htole32(bin.slot[i].unix_time);

	/* Read in command to execute
	*/
	printf("Enter the command to execute using the format: \"command\", \"arg\"\n");
	scanf(" %99[^\n\r]", bin.slot[i].command);
	int copied = snprintf(output, SCRIPT_MAX_LENGTH, "COMMAND(%s);", bin.slot[i].command);
	if( copied >= SCRIPT_MAX_LENGTH ) {
		printf("Command string too large, exiting scheduler..\n");
		return;
	}
	memcpy(bin.slot[i].command, output, strlen(output));
	printf("Scheduling command\n\t%s @time %" PRIu32 "\n",
	       bin.slot[i].command, bin.slot[i].unix_time);
	bin.slot[i].command_length = strlen(bin.slot[i].command);

	/* Update total commands in schedule.
	*/
	bin.total_commands = htole32((i+1));
}


int main( int argc, char** argv )
{
	(void) argc;
	(void) argv;

	printf("Ex-Alta 1 Flight Schedule Generator V1.1\n");

	for( i = 0; i < MAX_COMMANDS_PER_SCHEDULE; i++ ) {
		append_schedule(i);
		printf("Done? Press y to exit here, n to continue\n");
		for( ;; ) {
			is_exit = (char) getchar();
			if( is_exit == 'y' || is_exit == 'n' ) {
				break;
			}
		}
		if( is_exit == 'y' ) {
			break;
		}
		printf("\n");
	}
// Edit will include the option to change a sceduled command by choosing an index.
// Edit by Andrew R. (may 8, 2018)
char edit[1];
int  index_for_edit;

	//ask the operator to go back and overwrite a command they had previously created...
while(1){
	printf("Would you like to go back and edit any of the scheduled commands?\n(Y/N): ");
	scanf("%s", edit);
	    if(edit[0]=='Y'||edit[0]=='y'){
	        printf("Enter the index of the command you would like to change: ");
	        scanf("%d", &index_for_edit);
	        append_schedule(index_for_edit);
	        printf("\n");

	    }

	    else if(edit[0] == 'N'||edit[0]=='n'){
	    	printf("Done!\n");
	  break;
	    }
	    else{
	        printf("Please enter Y/N\n");
	    }
	}



/* Write the schedule into a file for upload to S/C.
	*/
	printf("Compiling flight schedule containing %d commands:\n", le32toh(bin.total_commands));
	fid = fopen("script.bin","w");
	if( fid == NULL ) {
		printf("Failed to create file for compiled schedule\n");
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

