/*
 * cmd_obc.c
 *
 *  Created on: 13/07/2010
 *      Author: oem
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>

#include <nanomind.h>

#include <util/console.h>
#include <util/timestamp.h>
#include <util/vermagic.h>
#include <util/log.h>
#include <csp/csp_endian.h>

//#include <adcs.h>


int cmd_obc_node(struct command_context *ctx) { 
	if (ctx->argc != 2) 
		return CMD_ERROR_SYNTAX; 
	obc_set_node(atoi(ctx->argv[1])); 
		return CMD_ERROR_NONE; 
}


int cmd_obc_load(struct command_context *ctx) {

	char * args = command_args(ctx);
	char path[100];

	if (sscanf(args, "%s", path) < 1 || !strnlen(path, 100))
		return CMD_ERROR_SYNTAX;

	printf("Path %s\r\n", path);

	obc_load_image(path);

	return CMD_ERROR_NONE;

}

int cmd_obc_jump_ram(struct command_context *ctx) {

	char * args = command_args(ctx);
	uint32_t addr;

	if (sscanf(args, "%"SCNx32, &addr) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Address %"PRIx32"\r\n", addr);

	obc_jump_ram(addr);

	return CMD_ERROR_NONE;

}

int cmd_obc_boot_conf(struct command_context *ctx) {

	char * args = command_args(ctx);
	unsigned int checksum;
	int boot_counts;
	char path[100];


	if (sscanf(args, "%x %d %s", &checksum, &boot_counts, path) != 3)
		return CMD_ERROR_SYNTAX;

	if ((boot_counts < 0) || (boot_counts > 100))
		return CMD_ERROR_SYNTAX;

	printf("Checksum 0x%X Boot counts %d path %s\r\n", checksum, boot_counts, path);


	obc_boot_conf(checksum, boot_counts, path);

	return CMD_ERROR_NONE;

}

int cmd_obc_boot_del(struct command_context *ctx) {

	printf("Deleting OBC boot configuration\r\n");

	obc_boot_del();

	return CMD_ERROR_NONE;

}

int cmd_obc_ram_to_rom(struct command_context *ctx) {

	char * args = command_args(ctx);
	uint32_t size, checksum;
	if (args == NULL || sscanf(args, "%"PRId32" %"PRIx32, &size, &checksum) < 2)
		return CMD_ERROR_SYNTAX;

	printf("Size: %"PRId32" checksum %#"PRIx32"\r\n", size, checksum);

	obc_ram_to_rom(size, checksum, 0x50000000, 0x48000000);

	return CMD_ERROR_NONE;

}

int cmd_obc_timesync(struct command_context *ctx) {

	char * args = command_args(ctx);
	//int32_t sec;
	time_t sec = 0;
	if (args == NULL || sscanf(args, "%"PRId32, &sec) != 1)
		return CMD_ERROR_SYNTAX;
/*
	time_t tt;
	timestamp_t t = {((sec == 0) ? time(0) : sec), 0};
	tt = t.tv_sec;
	printf("Set OBC time to: %s\r\n", ctime(&tt));
	obc_timesync(&t, 6000);
*/

	timestamp_t t;
	timestamp_t ret_t;
	if(sec == 1)
	{
		t.tv_sec = time(NULL);
	}
	else
	{
		t.tv_sec = sec;
	}
	t.tv_nsec = 0;
	ret_t = obc_timesync(&t, 6000);
	time_t tt = ret_t.tv_sec;
	printf("OBC time is now: %s Difference was %d seconds. (New-old)\r\n", ctime(&tt), ret_t.tv_nsec);

	return CMD_ERROR_NONE;

}

int cmd_obc_get_time(struct command_context *ctx) {

	timestamp_t t;
	timestamp_t ret_t;
	t.tv_sec = 0;
	t.tv_nsec = 0;
	ret_t = obc_timesync(&t, 6000);
	time_t tt = ret_t.tv_sec;
	printf("OBC time is: %s\r\n", ctime(&tt));

	return CMD_ERROR_NONE;

}

int cmd_obc_get_boot_count(struct command_context *ctx) {
	uint32_t boot_count = 0;
	if (obc_boot_count_get(&boot_count, 5000) >= 0)
		printf("Boot count: %"PRIu32"\r\n",boot_count);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}
int cmd_obc_reset_boot_count(struct command_context *ctx) {
	uint32_t boot_count = 0;
	if (obc_boot_count_reset(&boot_count, 5000) >= 0)
		printf("Boot count reset to: %"PRIu32"\r\n",boot_count);
	else
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_obc_adcs(struct command_context *ctx) {
if(ctx->argc!=4){
		return CMD_ERROR_SYNTAX;
	}

	FILE* fid;
	char buf[200]; // Commands must be less than 201 bytes total for radio link.
	uint8_t number = atoi(ctx->argv[1]);
	uint8_t length = atoi(ctx->argv[2]);
	uint8_t file_length = 0;
    uint8_t file_number = 0;
    uint8_t reply[2];
    if(number == 2)
    {
	memset(buf, 0, 200);
	buf[0] = number;

	timestamp_t t;
	t.tv_sec = time(NULL);
	t.tv_nsec = 0;

	memcpy(&buf[1], &t, 4);
	buf[5] = 0;
	buf[6] = 0;
	
    }
    else if(number < 128 && length > 0)
    {
	    fid = fopen(ctx->argv[3],"r");
	    fread(&file_length,1,1,fid);
        fread(&file_number,1,1,fid);
	    if(file_length != length || file_number != number)
        {
            return CMD_ERROR_SYNTAX;    
        }
	    fseek(fid, 1, SEEK_SET); //go back to file number entry to read in command
        fread(buf,1,length+1,fid); // length+1 to include command ID
        fclose(fid);
    }
    else if(number < 128 && length == 0)
    {
        memset(buf, 0, 200);
        buf[0] = number;
    }
    else if(number >= 128)
    {
        memset(buf, 0, 200);
        buf[0] = number;
    }
    else
    {
        return CMD_ERROR_SYNTAX;
    }

    int err = csp_transaction(CSP_PRIO_NORM, NODE_OBC, OBC_PORT_ADCS, 1200, buf, length+1, reply, 1); // Not network endian.
    if( err < 0 ) {
   		printf("Timeout sending ADCS command\n");
   	}
   	if( err == 0 ) {
   		printf("ADCS command response too large - %d\n", err);
   	}
   	if( err > 0 ) {
   		printf("ADCS command received with status %d - execution status pending\n", reply[0]);
   	}

    //printf("Reply ID: %d\n",reply[0]);
    //printf("Reply Error: %d\n",reply[1]);

	return CMD_ERROR_NONE;
}

int cmd_obc_dfgm(struct command_context *ctx) {
	uint8_t on_or_off = atoi(ctx->argv[1]);
	uint8_t reply[2];
	if(on_or_off == 1)
	{
		csp_transaction(CSP_PRIO_NORM, NODE_OBC, OBC_DFGM_TEST, 0, &on_or_off, 1, reply, 2); // Not network endian.
	}
	else if(on_or_off == 0)
	{
		csp_transaction(CSP_PRIO_NORM, NODE_OBC, OBC_DFGM_TEST, 0, &on_or_off, 1, reply, 2); // Not network endian.
	}
	else
	{	
		return CMD_ERROR_FAIL;
	}
	return CMD_ERROR_NONE;
}

command_t __sub_command obc_subcommands[] = {
	{
		.name = "node",
		.help = "Set OBC address in host table",
		.usage = "<node>",
		.handler = cmd_obc_node,
	},{
		.name = "jump",
		.help = "Jump to address",
		.usage = "<address>",
		.handler = cmd_obc_jump_ram,
	},{
		.name = "load",
		.help = "Load image to RAM",
		.usage = "<path>",
		.handler = cmd_obc_load,
	},{
		.name = "bootconf",
		.help = "Boot conf. boot_counts must be positive and leq 100",
		.usage = "<checksum> <boot_counts> <filename>",
		.handler = cmd_obc_boot_conf,
	},{
		.name = "bootdel",
		.help = "Remove boot conf",
		.handler = cmd_obc_boot_del,
	},{
		.name = "ramtorom",
		.help = "Copy image from RAM to ROM",
		.usage = "<size> <checksum>",
		.handler = cmd_obc_ram_to_rom,
	},{
		.name = "ts",
		.help = "Synchronize time. Value of 1 gets current system time automatically.",
		.usage = "<time>",
		.handler = cmd_obc_timesync,
	},{
		.name = "gt",
		.help = "Get time from NanoMind RTC",
		.handler = cmd_obc_get_time,
	},{
		.name = "bcget",
		.help = "Get obc boot counter",
		.handler = cmd_obc_get_boot_count,
	},{
		.name = "bcreset",
		.help = "Reset obc boot counter",
		.handler = cmd_obc_reset_boot_count,
	},{
		.name = "adcs",
		.help = "Asks NanoMind to communicate with ADCS. Takes command/telem number, length of send struct, and filename of command file.",
		.handler = cmd_obc_adcs,
	},{
		.name = "dfgm",
		.help = "Starts or stops DFGM test with raw mode save.",
		.handler = cmd_obc_dfgm,
	}
};

command_t __root_command obc_commands[] = {
	{
		.name = "obc",
		.help = "OBC subsystem",
		.chain = INIT_CHAIN(obc_subcommands),
	}
};

void cmd_obc_setup(void) {
	command_register(obc_commands);
}
