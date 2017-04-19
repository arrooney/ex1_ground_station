/**
 * NanoPower firmware
 *
 * @author Morten Bisgaard
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <inttypes.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <util/console.h>
#include <util/log.h>

#include <command/command.h>
#include <nanomind.h>



int cmd_clearscreen(struct command_context *ctx) {
	system("clear");
	return CMD_ERROR_NONE;
}

int cmd_delay(struct command_context *ctx){

	char command[22];
	memset(command,0,22);
	strncpy(command,"sleep 1",22);
	if(ctx->argc==1){

	}
	else if(ctx->argc==2){
		strncpy(&command[6],ctx->argv[1],16);
	}
	else{
		return CMD_ERROR_SYNTAX;
	}
	system(command);
	return CMD_ERROR_NONE;

}


int cmd_script(struct command_context *ctx) {

	if(ctx->argc!=2){
		return CMD_ERROR_SYNTAX;
	}

	char line[5000];
	char * status;
	int ret,i;
	int gotol=0;
	FILE * fid;
	fid=fopen(ctx->argv[1],"r");

	status=fgets(line,5000,fid);
	while(status!=NULL){
		strstrip(line);
		if(strncmp(line,"goto ",5)==0){
			sscanf(line,"goto %d",&gotol);
			rewind(fid);
			for(i=0;i<gotol; i++){
				status=fgets(line,5000,fid);
				if(status==NULL){
					break;
				}
			}
		}
		else{
			ret=command_run(line);
			if(ret!=CMD_ERROR_NONE){
				break;
			}
			status=fgets(line,5000,fid);
		}
	}

	fclose(fid);
	return CMD_ERROR_NONE;
}

int cmd_send_string(struct command_context *ctx) {

#define MAX_STRING 100

	if(ctx->argc!=2){
		return CMD_ERROR_SYNTAX;
	}

	char out_string[MAX_STRING] = {0};
	uint16_t string_length = strlen(ctx->argv[1]);
	if( string_length > MAX_STRING )
		return CMD_ERROR_SYNTAX;

    memcpy(out_string, ctx->argv[1], string_length);
    //printf("Sending %s \n", out_string);
	//printf("Size: %d \n", string_length);
	csp_transaction(CSP_PRIO_NORM, NODE_OBC, OBC_PORT_TELECOMMAND, 0, out_string, string_length, NULL, 0); // Not converted to network endian

	return CMD_ERROR_NONE;
}

int cmd_print_string( struct command_context *ctx )
{
	if( ctx->argc != 2 ) {
		return CMD_ERROR_SYNTAX;
	}
	printf(ctx->argv[1]);
	printf("\n");
	return CMD_ERROR_NONE;
}

int cmd_ax100_rst_wdt(struct command_context *ctx) {

	csp_transaction(CSP_PRIO_HIGH, 5, 9, 1000, NULL, 0, NULL, 0);

	return CMD_ERROR_NONE;
}


command_t __root_command clear_commands[] = {
	{
		.name = "clear",
		.help = "Clear Screen",
		.usage = "clear",
		.handler = cmd_clearscreen,
	}
};

command_t __root_command delay_commands[] = {
	{
		.name = "delay",
		.help = "Delay script for N seconds",
		.usage = "delay [seconds]",
		.handler = cmd_delay,
	}
};

command_t __root_command script_commands[] = {
	{
		.name = "gsh",
		.help = "run script file",
		.usage = "gsh <filename>",
		.handler = cmd_script,
	}
};

command_t __root_command send_string[] = {
	{
		.name = "exec",
		.help = "Send 'COMMAND(\"<command_name>\" [,\"<arg>\"]);' for nanomind telecommand.",
		.usage = "send_string 'string'",
		.handler = cmd_send_string,
	}
};

command_t __root_command print_string[] = {
		{
				.name = "print",
				.help = "print '<string to print>' | <string_to_print>",
				.usage = "print argument to console",
				.handler = cmd_print_string,
		}
};

command_t __root_command ax100_rst_wdt[] = {
	{
		.name = "ax100_rst_wdt",
		.help = "Poke the AX100 to reset ground watchdog",
		.usage = "ax100_rst_wdt",
		.handler = cmd_ax100_rst_wdt,
	}
};

void cmd_utils_setup(void) {
	command_register(clear_commands);
	command_register(delay_commands);
	command_register(script_commands);
	command_register(send_string);
	command_register(ax100_rst_wdt);
	command_register(print_string);
}
