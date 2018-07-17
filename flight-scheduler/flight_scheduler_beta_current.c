/*
 * schedule.c
 *
 *  Created on: May 8, 2018
 *      Author: andre
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
/* Edited may 8th to offer the ability to change a command in the struct before upload...*/




#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <endian.h>
#include <stdlib.h>
#define SCRIPT_MAX_LENGTH (4*20+3)
#define MAX_COMMANDS_PER_SCHEDULE (4*60+3)
#define TRUE 1
#define FALSE 0

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
char is_save[1];
FILE *outfile;
char name[50], filename[50];

void save_schedule(int index);
void append_schedule(int index, int put);
void open_schedule();
void compile_schedule();

void save_schedule(int index){
//the function will now ask if they want to save the structure contiaining the
	

	    // open file for writing
	//if(index == 0){
	//outfile = fopen (filename, "w");
	//}
	//else{
	//outfile = fopen(filename, "a");
	//}
	    if (outfile == NULL)
	    {
	        fprintf(stderr, "\nError opening file\n");
	        exit (1);
	    }

	    // write struct to file
	    fwrite (&bin.slot[index], (sizeof(struct command_t)), 1, outfile);

	    //fclose(outfile);

	    if(fwrite != 0){
	        printf("contents to file written successfully !\n"); 	    }
	    else
	        printf("error writing file !\n");
	}


void append_schedule(int index, int put){
	int i = index;
	int put_in = put;
	printf("----------This is slot %d of %d command slots----------\n", i, MAX_COMMANDS_PER_SCHEDULE);

	/* Read in unix execution time.
	*/
	printf("Enter unix time of command execution:\n");
	//printf("Enter a value : ");
	scanf("%" SCNu32, &bin.slot[i].unix_time);
	bin.slot[i].unix_time = htole32(bin.slot[i].unix_time);

	/* Read in command to execute
	*/
	printf("Enter the command to execute using the format: \"command\", \"arg\"\n");
	scanf(" %99[^\n\r]", bin.slot[i].command);
	int copied = snprintf(output, SCRIPT_MAX_LENGTH, "COMMAND(%s);", bin.slot[i].command);
	if( copied >= SCRIPT_MAX_LENGTH ) {
		printf("Command string too large, exiting scheduler..\n");
	}
	memcpy(bin.slot[i].command, output, strlen(output));
	printf("Scheduling command\n\t%s @time %" PRIu32 "\n",
	       bin.slot[i].command, bin.slot[i].unix_time);
	bin.slot[i].command_length = strlen(bin.slot[i].command);

	/* Update total commands in schedule.
	*/
	bin.total_commands = htole32((i+1));
	if (put_in == TRUE){
		save_schedule(i);
	}
	else {
}

}





void open_schedule(){
	char name_open[50], filename_open[50];
	int count=0;
	printf("What is the name of the schedule? (Format should be: schedule_'given name')\n");
	scanf("%s", name_open);
	sprintf(filename_open, "%s.dat", name_open);
    FILE *infile;
    struct command_t input;

    // Open person.dat for reading
    infile = fopen (filename_open, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit (1);
    }

    // read file contents till end of file
    printf("Here is what this schedule currently looks like:\n");
    while(fread(&input, sizeof(struct command_t), 1, infile)){

        printf ("Command index = %d; Command = %s Time = %d\n", count, input.command, input.unix_time);
        strcpy(bin.slot[count].command,input.command);
        ++count;
    }
	fclose(infile);
    bin.total_commands = count;
    int n;
    int new_time;
    for(n=0; n<count;n++){
    	printf("\nPlease supply the new unix time for command number %d, the command is: %s\n", n, bin.slot[n].command);
    	scanf("%d", &new_time);
    	bin.slot[n].unix_time = new_time;
    }
   printf("Schedule updated.\n");
}



void compile_schedule(){
	/* Write the schedule into a file for upload to S/C.
	*/
	printf("Compiling flight schedule containing %d commands:\n", le32toh(bin.total_commands));
	fid = fopen("script.bin","w");
	if( fid == NULL ) {
		printf("Failed to create file for compiled schedule\n");
		return;
	}
	size_t written = fwrite(&bin, sizeof(bin), 1, fid);
	if( written != 1 ) {
		printf("Failed to write compiled schedule to file\n");
		return;
	}
	fclose(fid);
	printf("Done!\n");
}

int main( int argc, char** argv )
{
	(void) argc;
	(void) argv;

	printf("Ex-Alta 1 Flight Schedule Generator V1.2 - SMELLY CAT\n");
int t_f;

	//give the option to open a previously saved command structure, view contents and change time stamps.
printf("\nwould you like to open a previously saved schedule? \n(Y/N): ");
char is_open[1];
scanf("%s", is_open);
if (is_open[0]=='y'||is_open[0]=='Y'){
	open_schedule();
	compile_schedule();
	return 0;
}
else {}

printf("\nWould you like to save the schedule you are about to make? \n(Y/N): ");
scanf("%s", is_save);

if (is_save[0]=='Y'||is_save[0]=='y'){
	t_f=TRUE;
	printf("\nWhat will you call this schedule?\n(enter up to 10 letter name):");
	scanf("%s", name);
	sprintf(filename, "schedule_%s.dat", name);
	outfile = fopen(filename, "w");
	}
else if (is_save[0] == 'n'||is_save[0] == 'N'){
	t_f=FALSE;
	printf("Commands will not be saved... \n");
	}
else{
	printf("Answer should have been Y/N. Continuing to schedule creation...\n");
	}


for( i = 0; i < MAX_COMMANDS_PER_SCHEDULE; i++ ) {
		append_schedule(i, t_f);
		printf("\nAre you done creating this schedule? \n(Y/N): ");
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

char edit[1];
int  index_for_edit;

while(1){
	printf("Would you like to go back and edit any of the scheduled commands?\n(Y/N): ");
	scanf("%s", edit);
	    if(edit[0]=='Y'||edit[0]=='y'){
	        printf("Enter the index of the command you would like to change: ");
	        scanf("%d", &index_for_edit);
	        append_schedule(index_for_edit, t_f);
	        printf("\n");

	    }

	    else if(edit[0] == 'N'||edit[0]=='n'){
	    	printf("Done!\n");
	    	compile_schedule();
	    	return 0;
	  break;
	    }
	    else{
	        printf("Please enter Y/N\n");
	    }
	}


printf("should not be here");


/* Write the schedule into a file for upload to S/C.
	*/
//	printf("Compiling flight schedule containing %d commands:\n", le32toh(bin.total_commands));
	//fid = fopen("script.bin","w");
//	if( fid == NULL ) {
//		printf("Failed to create file for compiled schedule\n");
//		return -1;
//	}
//	size_t written = fwrite(&bin, sizeof(bin), 1, fid);
//	if( written != 1 ) {
//		printf("Failed to write compiled schedule to file\n");
//		return -1;
//	}
//	fclose(fid);
///	printf("Done!\n");
//	return 0;
}