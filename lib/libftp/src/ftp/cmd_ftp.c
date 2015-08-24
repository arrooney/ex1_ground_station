/**
 * @file cmd_ftp.c
 * FTP Client Command interface
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <inttypes.h>
#include <libgen.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <ftp/ftp_client.h>
#include <ftp/ftp_types.h>

#include <util/console.h>
#include <util/timestamp.h>
#include <util/log.h>

#if 0
int ftp_upload(uint8_t host, uint8_t port, const char * path, uint8_t type, int chunk_size, uint32_t addr, const char * remote_path, uint32_t * size, uint32_t * checksum);
int ftp_status_request(void);
int ftp_data(int count);
#endif

/* Settings */
static unsigned int ftp_host = 1;
static unsigned int ftp_port = 9;
static unsigned int ftp_chunk_size = 190;
static unsigned int ftp_backend = 3; 		// Use newlib backend as standard

/* State variables */
static uint32_t ftp_size;

int cmd_ftp_set_backend(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	ftp_backend = atoi(ctx->argv[1]);

	return CMD_ERROR_NONE;
}

int cmd_ftp_set_host_port(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	ftp_host = atoi(ctx->argv[1]);
	ftp_port = atoi(ctx->argv[2]);

	return CMD_ERROR_NONE;

}

char *my_basename(char *path) {
    char *base = strrchr(path, '/');
    return base ? base+1 : path;
}

int cmd_ftp_download_file(struct command_context *ctx) {

	if ((ctx->argc < 2) || (ctx->argc > 3))
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH)
		return CMD_ERROR_SYNTAX;

	char * local_path;
	if (ctx->argc == 3)
		local_path = ctx->argv[2];
	else
		local_path = my_basename(remote_path);

	int status = ftp_download(ftp_host, ftp_port, local_path, ftp_backend, ftp_chunk_size, 0, 0, remote_path, &ftp_size);

	if (status != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}

	if (ftp_status_reply() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_crc() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}

	ftp_done(1);

	return CMD_ERROR_NONE;

}


int cmd_ftp_upload_file(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[2];
	char * local_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH || strlen(remote_path) < 3)
		return CMD_ERROR_SYNTAX;
	if (strlen(local_path) < 3)
		return CMD_ERROR_SYNTAX;


	int status = ftp_upload(ftp_host, ftp_port, local_path, ftp_backend, ftp_chunk_size,  0, remote_path, &ftp_size, NULL);
	if (status != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_status_request() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_data(0) != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_crc() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}

	ftp_done(0);

	return CMD_ERROR_NONE;

}

int cmd_ftp_download_mem(struct command_context *ctx) {

	if (ctx->argc != 4)
		return CMD_ERROR_SYNTAX;

	char * local_path = ctx->argv[1];
	int memaddr;
	int memsize;

	if (strlen(local_path) > FTP_PATH_LENGTH || strlen(local_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (sscanf(ctx->argv[2], "%x", &memaddr) != 1)
		return CMD_ERROR_SYNTAX;

	if (sscanf(ctx->argv[3], "%u", &memsize) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Downloading from addr 0x%X size 0x%X to file %s\r\n", memaddr, memsize, local_path);

	int status = ftp_download(ftp_host, ftp_port, local_path, 0, ftp_chunk_size, memaddr, memsize, NULL, &ftp_size);

	if (status != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}

	if (ftp_status_reply() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_crc() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}

	ftp_done(1);

	return CMD_ERROR_NONE;

}

int cmd_ftp_upload_mem(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	char * local_path = ctx->argv[1];
	int memaddr;

	if (strlen(local_path) > FTP_PATH_LENGTH || strlen(local_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (sscanf(ctx->argv[2], "%x", &memaddr) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Uploading from %s to 0x%X\r\n", local_path, memaddr);

	int status = ftp_upload(ftp_host, ftp_port, local_path, 0, ftp_chunk_size, memaddr, "", &ftp_size, NULL);
	if (status != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_status_request() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_data(0) != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}
	if (ftp_crc() != 0) {
		ftp_done(0);
		return CMD_ERROR_FAIL;
	}

	ftp_done(1);

	return CMD_ERROR_NONE;

}

int cmd_ftp_download_run(struct command_context *ctx) {
	if (ftp_status_reply() != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_crc(struct command_context *ctx) {
	if (ftp_crc() != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_zip(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	uint8_t action;
	if (!strcmp(ctx->argv[0], "zip")) {
		action = FTP_ZIP;
	} else {
		action = FTP_UNZIP;
	}
	char * src_path = ctx->argv[1];
	if (strlen(src_path) > FTP_PATH_LENGTH || strlen(src_path) < 3)
		return CMD_ERROR_SYNTAX;

	char * dest_path = ctx->argv[2];
	if (strlen(dest_path) > FTP_PATH_LENGTH || strlen(dest_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (ftp_zip(ftp_host, ftp_port, ftp_backend, src_path, dest_path, action) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_done(struct command_context *ctx) {
	if (ftp_done(1) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_ftp_ls(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH)
		return CMD_ERROR_SYNTAX;
	if (ctx->pipe_mode==1){
		if (ftp_list_pipe(ftp_host, ftp_port, ftp_backend, remote_path) != 0)
			return CMD_ERROR_FAIL;
	}
	else{
		if (ftp_list(ftp_host, ftp_port, ftp_backend, remote_path) != 0)
			return CMD_ERROR_FAIL;
	}
	return CMD_ERROR_NONE;

}

int cmd_ftp_remove(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	char * remote_path = ctx->argv[1];

	if (strlen(remote_path) > FTP_PATH_LENGTH)
		return CMD_ERROR_SYNTAX;

	if (ftp_remove(ftp_host, ftp_port, ftp_backend, remote_path) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}

int cmd_ftp_mkfs(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	uint8_t dev = atoi(ctx->argv[1]);

	if (ftp_mkfs(ftp_host, ftp_port, ftp_backend, dev) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}

int cmd_ftp_move(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	char * from_path = ctx->argv[1];
	if (strlen(from_path) > FTP_PATH_LENGTH || strlen(from_path) < 3)
		return CMD_ERROR_SYNTAX;

	char * to_path = ctx->argv[2];
	if (strlen(to_path) > FTP_PATH_LENGTH || strlen(to_path) < 3)
		return CMD_ERROR_SYNTAX;

	if (ftp_move(ftp_host, ftp_port, ftp_backend, from_path, to_path) != 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;

}

command_t __sub_command ftp_subcommands[] = {
	{
		.name = "done",
		.help = "done transfer",
		.handler = cmd_ftp_done,
	},{
		.name = "ls",
		.help = "list files",
		.usage = "<path>",
		.handler = cmd_ftp_ls,
	},{
		.name = "rm",
		.help = "rm files",
		.handler = cmd_ftp_remove,
	},{
		.name = "mkfs",
		.help = "make file system",
		.handler = cmd_ftp_mkfs,
	},{
		.name = "mv",
		.help = "move files",
		.handler = cmd_ftp_move,
	},{
		.name = "crc",
		.help = "crc transfer",
		.handler = cmd_ftp_crc,
	},{
		.name = "zip",
		.help = "zip file",
		.handler = cmd_ftp_zip,
	},{
		.name = "unzip",
		.help = "unzip file",
		.handler = cmd_ftp_zip,
	},{
		.name = "download_run",
		.help = "Continue download (send status reply)",
		.handler = cmd_ftp_download_run,
	},{
		.name = "server",
		.help = "set host and port",
		.usage = "<server> <port>",
		.handler = cmd_ftp_set_host_port,
	},{
		.name = "backend",
		.help = "Set filesystem backend",
		.usage = "<backend, 0=mem, 1=fat, 2=vfs, 3=newlib>",
		.handler = cmd_ftp_set_backend,
	},{
		.name = "upload_file",
		.help = "Upload file",
		.usage = "<local filename> <remote filename> ",
		.handler = cmd_ftp_upload_file,
	},{
		.name = "download_file",
		.help = "Download file",
		.usage = "<filename>",
		.handler = cmd_ftp_download_file,
	},{
		.name = "upload_mem",
		.help = "Upload to memory",
		.usage = "<local_file> <remote_addr>",
		.handler = cmd_ftp_upload_mem,
	},{
		.name = "download_mem",
		.help = "Download memory",
		.usage = "<local_file> <remote_addr> <length>",
		.handler = cmd_ftp_download_mem,
	},
};

command_t __root_command ftp_commands[] = {
	{
		.name = "ftp",
		.help = "FTP commands",
		.chain = INIT_CHAIN(ftp_subcommands),
	}
};

void cmd_ftp_setup(void) {
	command_register(ftp_commands);
}
