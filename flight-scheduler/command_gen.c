/*

This program reads from STDIN in a user-friendly way to build command files.
*/





#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <float.h>

#define COMMAND_MAX_LENGTH 200


FILE* fid;
char is_exit[1];
float input[COMMAND_MAX_LENGTH];
int type[COMMAND_MAX_LENGTH];
int command;
int length;
int tally = 0;
uint8_t count = 0;
int i;

void main( ) {

	printf("Ex-Alta 1 ACDS Command Generator V0.1\n");

    printf("Enter command ID number: ");
    scanf("%d", &command);
    printf("Enter command length in bytes: ");
    scanf("%d", &length);
    printf("\n");

    for(i=0;i<COMMAND_MAX_LENGTH;i++)
    {
        printf("Used %d bytes of 200\n", tally);
        printf("Enter number of bytes for value (1,2,4, 5 for float): ");
		scanf("%d", &type[i]);
        printf("Enter value: ");
		scanf("%f", &input[i]);
        tally = tally + type[i]; // Add -1 for float.
        if(type[i] == 5)
        {
            tally = tally - 1;
        }
        count = i+1;	    
        printf("Done? Press y to exit: ");
        scanf("%s", is_exit);
        if((strcmp(is_exit,"y") == 0))
        {
            break;
        }
    }

    fid = fopen("adcs_command.txt","w");
    fwrite(&length,1,1,fid);
    fwrite(&command,1,1,fid);
    
    for(i=0;i<count;i++)
    {
        if(type[i] == 1)
        {
            uint8_t buf = (uint8_t) input[i];
            fwrite(&buf,1,1,fid);
        }
        else if(type[i] == 2)
        {
            uint16_t buf = (uint16_t) input[i];
            fwrite(&buf,2,1,fid);
        }
        else if(type[i] == 4)
        {
            uint32_t buf = (uint32_t) input[i];
            fwrite(&buf,4,1,fid);
        }
        else if(type[i] == 5)
        {
            float buf = (float) input[i];
            fwrite(&buf,4,1,fid);
        }
    }
    fclose(fid);

	printf("Done!\n");
	return;
}
