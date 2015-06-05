#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <command/command.h>
#include <util/vmem.h>
#include <util/log.h>
#include <util/hexdump.h>
#include <util/console.h>

int cmd_vmem_read(struct command_context * ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	vmemptr_t addr;
	sscanf(ctx->argv[1], "%lx", &addr);
	unsigned int length = atoi(ctx->argv[2]);

	char data[length];
	vmem_cpy((vmemptr_t) (uintptr_t) data, (vmemptr_t) addr, length);
	hex_dump(data, length);
	return CMD_ERROR_NONE;
}

int cmd_vmem_write(struct command_context * ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	vmemptr_t addr;
	sscanf(ctx->argv[1], "%lx", &addr);

	char data[CONSOLE_BUFSIZ/2];
	int len = strlen(ctx->argv[2]) / 2;

	unsigned int to_int(char c) {
		if (c >= '0' && c <= '9') return      c - '0';
		if (c >= 'A' && c <= 'F') return 10 + c - 'A';
		if (c >= 'a' && c <= 'f') return 10 + c - 'a';
		return -1;
	}

	for (int i = 0; ((i < len) && (i < CONSOLE_BUFSIZ/2)); ++i)
		data[i] = 16 * to_int(ctx->argv[2][2*i]) + to_int(ctx->argv[2][2*i+1]);


	vmem_cpy((vmemptr_t) addr, (vmemptr_t) (uintptr_t) data, len);
	return CMD_ERROR_NONE;
}

int cmd_vmem_list(struct command_context * ctx) {
	vmem_list();
	return CMD_ERROR_NONE;
}

command_t __sub_command vmem_subcommands[] = {
	{
		.name = "read",
		.help = "Read from VMEM",
		.usage = "<addr> <length>",
		.handler = cmd_vmem_read,
	},{
		.name = "write",
		.help = "Write to VMEM",
		.usage = "<addr> <data>",
		.handler = cmd_vmem_write,
	},{
		.name = "list",
		.help = "Show VMEMs",
		.handler = cmd_vmem_list,
	}
};

command_t __root_command vmem_commands[] = {
	{
		.name = "vmem",
		.help = "Virtual memory",
		.chain = INIT_CHAIN(vmem_subcommands),
	}
};
