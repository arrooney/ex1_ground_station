/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include <command/command.h>
#include <util/console.h>
#include <util/log.h>
#include <uthash/utlist.h>

#include <param/param_types.h>
#include <param/param_memio.h>
#include <param/param_string.h>
#include <param/param_serializer.h>
#include <param_host.h>
#include <param_index.h>

 param_mem cmd_param_mem = 0;
 int cmd_param_verbose = 0;

int cmd_param_set_mem(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	cmd_param_mem = atoi(ctx->argv[1]);
	if (cmd_param_verbose)
		printf("Using param mem %u\r\n", cmd_param_mem);
	return CMD_ERROR_NONE;
}

int cmd_param_set_verbose(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	cmd_param_verbose = atoi(ctx->argv[1]);
	printf("Param verbose %d\r\n", cmd_param_verbose);
	return CMD_ERROR_NONE;
}

int cmd_param_list(struct command_context *ctx) {

	if (ctx->argc >= 4)
		return CMD_ERROR_SYNTAX;

	int mem = cmd_param_mem;
	if (ctx->argc >= 2)
		mem = atoi(ctx->argv[1]);

	int do_read = 1;
	if (ctx->argc >= 3)
		do_read = atoi(ctx->argv[2]);

	printf("Parameter list %u:\r\n", mem);
	param_list(param_ptr(mem), do_read);

	return CMD_ERROR_NONE;

}

int cmd_param_tables(struct command_context *ctx) {

	for (int i = 0; i < PARAM_MAX_TABLES; i++) {
		param_index_t * idx = &param_index[i];
		if (idx->table == NULL)
			continue;
		printf("%02"PRIu8": cnt %03u, mem %10p, sz %03"PRIu32", fram 0x%04"PRIx16", cks 0x%04"PRIx16, idx->mem_id, idx->count, idx->physaddr, idx->size, idx->framaddr, idx->table_chksum);
		if (idx->scratch_size > 0)
			printf(", scratch %u", idx->scratch_size);
		if (idx->pool != NULL) {
			param_pool_t * el;
			unsigned int count;
			DL_COUNT(idx->pool, el, count);
			printf(", pool [elm %03u cnt %u sz %lu]", idx->pool_packsize, count, (idx->pool_packsize + sizeof(param_pool_t)) * count);
		}
		printf("\r\n");

	}

	return CMD_ERROR_NONE;

}

int cmd_param_set(struct command_context *ctx) {
	if (ctx->argc < 3)
		return CMD_ERROR_SYNTAX;

	/* Try parsing parameter as a string */
	char shortname[strlen(ctx->argv[1])];
	int array_index = -1;
	sscanf(ctx->argv[1], "%[^[][%d]", shortname, &array_index);

	param_index_t * mem = param_ptr(cmd_param_mem);
	const param_table_t * param = param_find_name(mem->table, mem->count, shortname);

	/* If not found, try parsing as a hex addr */
	if (param == NULL) {
		int param_addr = -1;
		sscanf(ctx->argv[1], "%x", &param_addr);
		if (param_addr >= 0)
			param = param_find_addr(mem->table, mem->count, param_addr);
	}

	if (param == NULL) {
		printf("Unknown parameter %s\r\n", ctx->argv[1]);
		return CMD_ERROR_FAIL;
	}

	if (array_index >= PARAM_COUNT) {
		if (PARAM_COUNT > 1)
			printf("Array index out of bounds, max index = %u\r\n", PARAM_COUNT - 1);
		else
			printf("Parameter is not an array\r\n");
		return CMD_ERROR_FAIL;
	}

	if (array_index >= 0) {

		if (ctx->argc != 3) {
			printf("Use only one argument for array offsets");
			return CMD_ERROR_FAIL;
		}

		/* Parse input */
		uint8_t value[PARAM_SIZE];
		if (param_from_string(param, ctx->argv[2], value) < 0)
			return CMD_ERROR_INVALID;

		/* Set to memory */
		param_deserialize_item(param, PARAM_ADDR + (PARAM_SIZE * array_index), param_ptr(cmd_param_mem), value, (cmd_param_verbose) ? F_PRINT_GET : 0);

	} else {

		/* Check count for arrays */
		if ((PARAM_COUNT >= 2) && (ctx->argc - 2 > PARAM_COUNT)) {
			printf("Too many arguments, max %u\r\n", PARAM_COUNT);
			return CMD_ERROR_FAIL;
		}

		/* Check for multiple arguments to a non-array */
		if ((PARAM_COUNT <= 1) && (ctx->argc > 3)) {
			printf("Parameter is not an array\r\n");
			return CMD_ERROR_FAIL;
		}

		for (int i = 0; i < (ctx->argc - 2); i++) {

			/* Parse input */
			uint8_t value[PARAM_SIZE];
			if (param_from_string(param, ctx->argv[i + 2], value) < 0)
				return CMD_ERROR_INVALID;

			/* Set to memory */
			param_deserialize_item(param, PARAM_ADDR + (PARAM_SIZE * i), param_ptr(cmd_param_mem), value, (cmd_param_verbose) ? F_PRINT_GET : 0);

		}

	}

	return CMD_ERROR_NONE;

}

#ifndef __AVR__
int cmd_param_completer(char *line) {

	int matchlen(const char * a, const char * b, int maxlen) {
		int i = 0;
		while(i < maxlen) {
			if (a[i]!=b[i])
				break;
			i++;
		}
		//printf("Match %s = %s %d\r\n", a, b, i);
		return i;
	}

	if (param_ptr(cmd_param_mem)->table == NULL)
		return CMD_ERROR_NOTFOUND;

	const param_table_t * param = NULL;
	int multiple = 0;
	int min_matchlen = 100;
	for (unsigned int i = 0; i < param_ptr(cmd_param_mem)->count; i++) {
		if (strncmp(line, param_ptr(cmd_param_mem)->table[i].name, strlen(line)) == 0) {
			if (param) {
				if (multiple == 0) {
					printf("\r\n");
					param_list_single(param, param_ptr(cmd_param_mem), 1);
				}
				multiple = 1;
				param_list_single(&param_ptr(cmd_param_mem)->table[i], param_ptr(cmd_param_mem), 1);

				int new_matchlen = matchlen(param->name, param_ptr(cmd_param_mem)->table[i].name, strlen(param->name));
				if (new_matchlen < min_matchlen) {
					min_matchlen = new_matchlen;
					param = &param_ptr(cmd_param_mem)->table[i];
				}
			} else {
				param = &param_ptr(cmd_param_mem)->table[i];
				min_matchlen = strlen(param->name);
			}

		}
	}

	if ((min_matchlen == 0) || (param == NULL))
		return CMD_ERROR_RICHAMBIG;

	memset(line, 0, min_matchlen + 1);
	strncpy(line, param->name, min_matchlen);
	return CMD_ERROR_EXTENDED;

}
#endif

int cmd_param_get(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	/* Try parsing parameter as a string */
	char shortname[strlen(ctx->argv[1])];
	int array_index = -1;
	sscanf(ctx->argv[1], "%[^[][%d]", shortname, &array_index);

	param_index_t * mem = param_ptr(cmd_param_mem);
	const param_table_t * param = param_find_name(mem->table, mem->count, shortname);

	/* If not found, try parsing as a hex addr */
	if (param == NULL) {
		int param_addr = -1;
		sscanf(ctx->argv[1], "%x", &param_addr);
		if (param_addr >= 0)
			param = param_find_addr(mem->table, mem->count, param_addr);
	}

	if (param == NULL) {
		printf("Unknown parameter %s\r\n", ctx->argv[1]);
		return CMD_ERROR_FAIL;
	}

	if (array_index >= PARAM_COUNT) {
		if (PARAM_COUNT > 1)
			printf("Array index out of bounds, max index = %u\r\n", PARAM_COUNT - 1);
		else
			printf("Parameter is not an array\r\n");
		return CMD_ERROR_FAIL;
	}

	uint8_t string[100];
	if (array_index >= 0) {
		printf("  GET %s[%u] = ", shortname, array_index);
		void * addr = param_read_addr(PARAM_ADDR + (PARAM_SIZE * array_index), param_ptr(cmd_param_mem), PARAM_SIZE);
		param_lock(mem);
		param_to_string(param, string, 0, addr, 0, 100);
		param_unlock(mem);
		printf("%s ", string);
	} else {
		printf("  GET %s = ", shortname);
		int count = PARAM_COUNT;
		if (count == 0)
			count = 1;

		for (int i = 0; i < count; i++) {
			void * addr = param_read_addr(PARAM_ADDR + (PARAM_SIZE * i), param_ptr(cmd_param_mem), PARAM_SIZE);
			param_lock(mem);
			param_to_string(param, string, 0, addr, 0, 100);
			param_unlock(mem);
			printf("%s ", string);
		}
	}

	printf("\r\n");

	return CMD_ERROR_NONE;

}

int cmd_param_copy(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	int from = atoi(ctx->argv[1]);
	int to = atoi(ctx->argv[2]);

	param_copy(param_ptr(from), param_ptr(to), 0);
	return CMD_ERROR_NONE;
}

int cmd_param_load(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	int file_id = atoi(ctx->argv[1]);
	int to = atoi(ctx->argv[2]);

	if (param_load(file_id, param_ptr(to)) < 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_param_save(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	int from = atoi(ctx->argv[1]);
	int file_id = atoi(ctx->argv[2]);

	if (param_save(param_ptr(from), file_id) < 0)
		return CMD_ERROR_FAIL;
	return CMD_ERROR_NONE;
}

int cmd_param_clear(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	int file_id = atoi(ctx->argv[1]);

	param_clear(file_id);
	return CMD_ERROR_NONE;
}

int cmd_param_export(struct command_context *ctx) {

	if (ctx->argc >= 3)
		return CMD_ERROR_SYNTAX;

	int mem = cmd_param_mem;
	if (ctx->argc >= 2)
		mem = atoi(ctx->argv[1]);

	printf("param mem %d\r\n", mem);

	param_index_t * idx = param_ptr(mem);
	for (unsigned int i = 0; i < idx->count; i++) {
		const param_table_t * param = &idx->table[i];

		printf("param set ");

		char buf[MAX_PARAM_NAME_LEN + 1] = {};
		param_memcpy(buf, param->name, MAX_PARAM_NAME_LEN);
		printf("%-16s\t", buf);

		int count = PARAM_COUNT;
		if (count == 0)
			count = 1;

		for (int j = 0; j < count; j++) {
			uint8_t string[100];
			param_to_string(param, string, 0, param_read_addr(PARAM_ADDR + PARAM_SIZE * j, param_ptr(mem), PARAM_SIZE), 0,100);
			printf(" %s", string);
		}

		printf("\r\n");

	}
	return CMD_ERROR_NONE;

}

command_t __sub_command param_subcommands[] = {
	{
		.name = "mem",
		.help = "Set cmds working mem",
		.usage = "<mem>",
		.handler = cmd_param_set_mem,
	},{
		.name = "list",
		.help = "List parameters",
		.usage = "[mem] [doread]",
		.handler = cmd_param_list,
	},{
		.name = "tables",
		.help = "List tables",
		.handler = cmd_param_tables,
	},{
		.name = "export",
		.help = "Export parameters",
		.usage = "[mem]",
		.handler = cmd_param_export,
	},{
		.name = "set",
		.usage = "<name|addr> <value> [quiet]",
		.help = "Set parameter",
		.handler = cmd_param_set,
#ifndef __AVR
		.completer = cmd_param_completer,
#endif
	},{
		.name = "get",
		.usage = "<name|addr>",
		.help = "Get parameter",
		.handler = cmd_param_get,
#ifndef __AVR
		.completer = cmd_param_completer,
#endif
	},{
		.name = "copy",
		.usage = "<from_mem> <to_mem>",
		.help = "Copy parameter sets from mem to mem",
		.handler = cmd_param_copy,
	},{
		.name = "load",
		.usage = "<from_file> <to_mem>",
		.help = "Load config from file/fram to mem",
		.handler = cmd_param_load,
	},{
		.name = "save",
		.usage = "<from_mem> <to_file>",
		.help = "Save config from mem to file/fram",
		.handler = cmd_param_save,
	},{
		.name = "clear",
		.usage = "<file>",
		.help = "Clear config from file/fram",
		.handler = cmd_param_clear,
	},{
		.name = "verbose",
		.usage = "<0|1>",
		.help = "Turn verbose on/off",
		.handler = cmd_param_set_verbose,
	}
};

command_t __root_command param_commands[] = {
	{
		.name = "param",
		.help = "Parameter commands",
		.chain = INIT_CHAIN(param_subcommands),
	},
};

void cmd_param_setup(void) {
	command_register(param_commands);
}
