/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <util/log.h>

#include <command/command.h>
#include <csp/csp_endian.h>
#include <csp/csp.h>

#include <param/param_serializer.h>
#include <param/param_string.h>
#include <param/param_types.h>
#include <param/param_fletcher.h>
#include <rparam_client.h>

/** Local storage for query */
static uint8_t query_buf[RPARAM_QUERY_MAX_LEN+10] = {0};
static rparam_query * query = (void *) &query_buf;
static int query_get_size = 0;

/** Remote param system setup */

static param_index_t rparam_mem_i = {0};
static uint16_t rparam_checksum = 0;
static int rparam_node = 0;
static int rparam_port = 7;
static int rparam_autosend = 1;

int cmd_rparam_set_autosend(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	rparam_autosend = atoi(ctx->argv[1]);
	printf("rparam autosend set to %u\r\n", rparam_autosend);
	return CMD_ERROR_NONE;
}

int cmd_rparam_list(struct command_context *ctx) {

	if (rparam_mem_i.table == NULL)
		return CMD_ERROR_FAIL;

	param_list(&rparam_mem_i, 1);
	return CMD_ERROR_NONE;

}

int cmd_rparam_init(struct command_context *ctx) {

	printf("argc=%d\n",ctx->argc);
	int i=0;
	for(i=0;i<(ctx->argc);i++){
		printf("argv[%d]=%s\n",i,ctx->argv[i]);
	}


	if ((ctx->argc != 2) && (ctx->argc != 3))
		return CMD_ERROR_SYNTAX;

	if (ctx->argc >= 3)
		rparam_mem_i.mem_id = atoi(ctx->argv[2]);

	/** Local storage of parameters */
	if (rparam_mem_i.physaddr == NULL) {
		rparam_mem_i.physaddr = calloc(0x400, 1);
		rparam_mem_i.size = 0x400;
	}

	/** Local storage of file referene */
	static void * rparam_file = NULL;

	rparam_node = atoi(ctx->argv[1]);

	char fname[16];
	sprintf(fname, "param-%u-%u.bin", rparam_node, rparam_mem_i.mem_id);

	FILE * fd = fopen(fname, "r");
	if (fd == NULL) {
		printf("Unknown parameter list or file: %s\r\n", fname);
		return CMD_ERROR_INVALID;
	}

	if (rparam_file != NULL)
		free(rparam_file);
	rparam_file = calloc(10000, 1);
	if (rparam_file == NULL)
		return CMD_ERROR_NOMEM;

	int bytes = 0;

	bytes = fread(&rparam_checksum, 1, sizeof(uint16_t), fd);
	printf("Read %d bytes from %s\r\n", bytes, ctx->argv[1]);
	bytes = fread(rparam_file, 1, 10000, fd);
	printf("Read %d bytes from %s\r\n", bytes, ctx->argv[1]);
	fclose(fd);

	rparam_mem_i.table = (param_table_t *) rparam_file;
	rparam_mem_i.count = bytes / sizeof(param_table_t);

	printf("Rparam setup for node %u and port %u\r\n", rparam_node, rparam_port);

	/* List downloaded parameters */
	param_list(&rparam_mem_i, 0);

	return CMD_ERROR_NONE;
}

int cmd_rparam_download(struct command_context *ctx) {

	printf("argc:%d\n",ctx->argc);
	uint8_t i;
	for(i=0; i<ctx->argc; i++){
		printf("argv[%d]=%s\n",i,ctx->argv[i]);
	}

	if ((ctx->argc != 2) && (ctx->argc != 3))
		return CMD_ERROR_SYNTAX;

	if (rparam_mem_i.physaddr == NULL) {
		rparam_mem_i.physaddr = calloc(0x400, 1);
		if (rparam_mem_i.physaddr != NULL)
			rparam_mem_i.size = 0x400;
	}

	if (rparam_mem_i.physaddr == NULL) {
		log_error("Not enough memory for rparam client");
		return CMD_ERROR_FAIL;
	}

	rparam_node = atoi(ctx->argv[1]);
	if (ctx->argc >= 3)
		rparam_mem_i.mem_id = atoi(ctx->argv[2]);

	printf("Downloading RPARAM table %u for node %u on port %u\r\n", rparam_mem_i.mem_id, rparam_node, rparam_port);

	query->action = RPARAM_TABLE;
	query->length = 0;
	query->mem = rparam_mem_i.mem_id;

	csp_conn_t * conn = csp_connect(CSP_PRIO_HIGH, rparam_node, rparam_port, 10000, CSP_O_CRC32);
	if (conn == NULL)
		return CMD_ERROR_FAIL;

	/* Allocate outgoing buffer */
	csp_packet_t * packet = csp_buffer_get(RPARAM_QUERY_MAX_LEN);
	if (packet == NULL) {
		csp_close(conn);
		return CMD_ERROR_FAIL;
	}

	/* Copy data to send */
	packet->length = query->length + offsetof(rparam_query, packed);
	memcpy(packet->data, query, packet->length);

	/* Send packet */
	if (!csp_send(conn, packet, 0)) {
		csp_buffer_free(packet);
		csp_close(conn);
		return CMD_ERROR_FAIL;
	}

	/* Receive remote parameter table, in host byte order
	 * Note: This is necessary, because the SFP functions does not know the dataformat
	 * and hence cannot be converted server-side. */
	void * dataout = NULL;
	int totalsize = 0;
	int result = csp_sfp_recv(conn, &dataout, &totalsize, 10000);
	csp_close(conn);

	if (result < 0) {
		if (dataout != NULL)
			free(dataout);
		return CMD_ERROR_FAIL;
	}

	/* Set new rparam table */
	if (rparam_mem_i.table != NULL)
		free((void *) rparam_mem_i.table);

	param_table_t * table = dataout;
	uint8_t count = totalsize / sizeof(param_table_t);

	/* Calculate checksum on table (before converting endians!) */
	rparam_checksum = fletcher16((void *) table, totalsize);
	printf("Checksum is: 0x%X\r\n", rparam_checksum);

	/* Autodetect Endians */
	int sum_first = 0, sum_last = 0;
	for (int i = 0; i < count; i++) {
		sum_first += (table[i].addr & 0xFF00) >> 8;
		sum_last += table[i].addr & 0xFF;
	}

	/* Correct endians */
	if (sum_first > sum_last) {
		for (int i = 0; i < count; i++) {
			table[i].addr = (((table[i].addr & 0xff00) >> 8) | ((table[i].addr & 0x00ff) << 8));
		}
	}

	/* Store table locally */
	rparam_mem_i.table = table;
	rparam_mem_i.count = count;

	/* Store parameter file to file */
	char fname[16];
	sprintf(fname, "param-%u-%u.bin", rparam_node, rparam_mem_i.mem_id);
	FILE * fd = fopen(fname, "w");
	if (fd != NULL) {
		int count = 0;
		count = fwrite(&rparam_checksum, 1, sizeof(uint16_t), fd);
		printf("Wrote %u bytes to %s\r\n", count, fname);
		count = fwrite(rparam_mem_i.table, 1, totalsize, fd);
		printf("Wrote %u bytes to %s\r\n", count, fname);
		fclose(fd);
	}

	/* List downloaded parameters */
	param_list(&rparam_mem_i, 0);

	return CMD_ERROR_NONE;

}

int cmd_rparam_send(struct command_context *ctx) {

	int override_crc = 0;
	if (ctx->argc == 2) {
		override_crc = atoi(ctx->argv[1]);
	} else if (ctx->argc > 2) {
		return CMD_ERROR_SYNTAX;
	}

	if (rparam_mem_i.table == NULL)
		return CMD_ERROR_FAIL;

	if (query->length == 0)
		query->action = RPARAM_GET;

	/* Allocate outgoing buffer */
	csp_packet_t * packet = csp_buffer_get(RPARAM_QUERY_MAX_LEN);
	if (packet == NULL)
		return CMD_ERROR_FAIL;

	csp_conn_t * conn = csp_connect(CSP_PRIO_HIGH, rparam_node, rparam_port, 10000, CSP_O_CRC32);

	/* Copy data to send */
	packet->length = query->length + offsetof(rparam_query, addr);
	memcpy(packet->data, query, packet->length);
	rparam_query * net_query = (rparam_query *) packet->data;

	/* Calculate checksum */
	if (override_crc == 0) {
		net_query->checksum = rparam_checksum;
	} else {
		net_query->checksum = 0xB00B;
	}

	/* Set mem */
	net_query->mem = rparam_mem_i.mem_id;

	/* Deal with endianness */
	if (query->action == RPARAM_GET)
		for (int i = 0; i < query->length; i++)
			net_query->addr[i] = csp_hton16(net_query->addr[i]);
	net_query->length = csp_hton16(net_query->length);
	net_query->checksum = csp_hton16(net_query->checksum);

	/* Send packet */
	if (!csp_send(conn, packet, 0)) {
		printf("Failed to send query\r\n");
		csp_buffer_free(packet);
		csp_close(conn);
		return CMD_ERROR_FAIL;
	}

	/* Read reply */
	packet = csp_read(conn, 1000);
	if (packet == NULL) {
		printf("No reply\r\n");
		csp_close(conn);
		return CMD_ERROR_FAIL;
	}

	if (packet->length <= 1) {
		if (packet->length == 1) {
			printf("Result: %u\r\n", packet->data[0]);
		} else {
			printf("Error: empty reply\r\n");
		}
		csp_buffer_free(packet);
		csp_close(conn);
		return CMD_ERROR_NONE;
	}

	/* We have a reply */
	rparam_query * reply = (rparam_query *) packet->data;
	reply->length = csp_ntoh16(reply->length);

	if (reply->action == RPARAM_REPLY) {
		param_deserialize(&rparam_mem_i, reply->packed, reply->length, F_FROM_BIG_ENDIAN | F_PRINT_REP);
	}

	csp_buffer_free(packet);
	csp_close(conn);
	return CMD_ERROR_NONE;

}

int cmd_rparam_get(struct command_context *ctx) {

	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;

	if (rparam_mem_i.table == NULL)
		return CMD_ERROR_FAIL;

	/* Ensure correct header */
	if (query->action != RPARAM_GET) {
		query->length = 0;
		query->action = RPARAM_GET;
		query_get_size = 0;
	}

	char shortname[strlen(ctx->argv[1])];
	int array_index = -1;
	sscanf(ctx->argv[1], "%[^[][%d]", shortname, &array_index);

	const param_table_t * param = param_find_name(rparam_mem_i.table, rparam_mem_i.count, shortname);
	if (param == NULL) {
		printf("Unknown parameter %s, check 'rparam list' for names\r\n", ctx->argv[1]);
		return CMD_ERROR_INVALID;
	}

	if (array_index >= param->count) {
		printf("Array index out of bounds, max index = %u\r\n", param->count - 1);
		return CMD_ERROR_FAIL;
	}

	if (array_index >= 0) {

		/* Size check */
		if (query_get_size + param->size + sizeof(uint16_t) > RPARAM_QUERY_MAX_LEN)
			return CMD_ERROR_FAIL;

		/* Add to query */
		query->addr[query->length/2] = param->addr + (param->size * array_index);
		query->length += sizeof(uint16_t);
		query_get_size += param->size + sizeof(uint16_t);

	} else {

		int count = param->count;
		if (count == 0)
			count = 1;

		for (int i = 0; i < count; i++) {

			/* Size check */
			if (query_get_size + param->size + sizeof(uint16_t) > RPARAM_QUERY_MAX_LEN)
				return CMD_ERROR_FAIL;

			/* Add to query */
			query->addr[query->length/2] = param->addr + (param->size * i);
			query->length += sizeof(uint16_t);
			query_get_size += param->size + sizeof(uint16_t);

		}

	}

	/* Perform dry run serialization on table */
	unsigned int start = 0;
	param_serialize_list(&rparam_mem_i, query->addr, query->length/2, &start, NULL, RPARAM_QUERY_MAX_LEN, F_DRY_RUN | F_PRINT_GET);

	/* Autosend */
	if (rparam_autosend) {
		struct command_context ctx = { .argc = 0 };
		int result = cmd_rparam_send(&ctx);
		query->action = RPARAM_GET;
		query->length = 0;
		query_get_size = 0;
		return result;
	}

	return CMD_ERROR_NONE;

}

int cmd_rparam_getall(struct command_context *ctx) {
	int result = rparam_get_full_table(&rparam_mem_i, rparam_node, rparam_port, rparam_mem_i.mem_id, 1000);
	if (result == 0)
		param_list(&rparam_mem_i, 1);
	return CMD_ERROR_NONE;
}

int cmd_rparam_set(struct command_context *ctx) {

	if (ctx->argc < 3)
		return CMD_ERROR_SYNTAX;


	if (rparam_mem_i.table == NULL)
		return CMD_ERROR_FAIL;


	printf("query->action %d, query->length %d\n",query->action,query->length);

	/* Ensure correct header */
	if (query->action != RPARAM_SET) {
		query->length = 0;
		query->action = RPARAM_SET;
	}
	printf("query->action %d, query->length %d\n",query->action,query->length);

	char shortname[strlen(ctx->argv[1])];
	int array_index = -1;
	sscanf(ctx->argv[1], "%[^[][%d]", shortname, &array_index);


	const param_table_t * param = param_find_name(rparam_mem_i.table, rparam_mem_i.count, shortname);
	if (param == NULL) {
		printf("Unknown parameter %s\r\n", ctx->argv[1]);
		return CMD_ERROR_FAIL;
	}

	if (array_index >= param->count) {
		printf("Array index out of bounds, max index = %u\r\n", param->count - 1);
		return CMD_ERROR_FAIL;
	}

	printf("array_index: %d",array_index);

	if (array_index >= 0) {

		if (ctx->argc != 3) {
			printf("Use only one argument for array offsets");
			return CMD_ERROR_FAIL;
		}

		/* Parse input */
		uint8_t value[param->size];
		if (param_from_string(param, ctx->argv[2], value) < 0)
			return CMD_ERROR_INVALID;

		/* Actual set query */
		param_serialize_item(param, param->addr + (param->size * array_index), query->packed, &query->length, RPARAM_QUERY_MAX_LEN, value, F_TO_BIG_ENDIAN);

		uint8_t string[100];
		param_to_string(param, string, 0, value, 0, 100);
		printf("  INP %s[%u] = %s\r\n", shortname, array_index, string);

	} else {

		/* Check count for arrays */
		if ((param->count != 0) && (ctx->argc - 2 > param->count)) {
			printf("Too many arguments for %s[%u]\r\n", param->name, param->count);
			return CMD_ERROR_FAIL;
		}

		for (int i = 0; i < (ctx->argc - 2); i++) {

			/* Parse input */
			uint8_t value[param->size];
			if (param_from_string(param, ctx->argv[i + 2], value) < 0)
				return CMD_ERROR_INVALID;

			/* Actual set query */
			param_serialize_item(param, param->addr + (param->size * i), query->packed, &query->length, RPARAM_QUERY_MAX_LEN, value, F_TO_BIG_ENDIAN);

			uint8_t string[100];
			param_to_string(param, string, 0, value, 0, 100);

			if (param->count > 1) {
				printf("  INP %s[%u] = %s\r\n", param->name, i, string);
			} else {
				printf("  INP %s = %s\r\n", param->name, string);
			}

		}

	}

	/* Autosend */
	if (rparam_autosend) {
		struct command_context ctx = { .argc = 0 };
		int result = cmd_rparam_send(&ctx);
		query->action = RPARAM_GET;
		query->length = 0;
		query_get_size = 0;
		return result;
	}

	return CMD_ERROR_NONE;

}

int cmd_rparam_print(struct command_context *ctx) {

	unsigned int start = 0;

	if (rparam_mem_i.table == NULL)
		return CMD_ERROR_FAIL;

	switch(query->action) {
	case RPARAM_GET:
		param_serialize_list(&rparam_mem_i, query->addr, query->length/2, &start, NULL, RPARAM_QUERY_MAX_LEN, F_DRY_RUN | F_TO_BIG_ENDIAN | F_PRINT_GET);
		break;
	case RPARAM_SET:
		param_deserialize(&rparam_mem_i, query->packed, query->length, F_DRY_RUN | F_FROM_BIG_ENDIAN | F_PRINT_SET);
		break;
	case RPARAM_REPLY:
		param_deserialize(&rparam_mem_i, query->packed, query->length, F_DRY_RUN | F_FROM_BIG_ENDIAN | F_PRINT_REP);
		break;
	case RPARAM_COPY:
		printf("  COPY from <%u> to <%u>\r\n", query->copy.from, query->copy.to);
		break;
	case RPARAM_LOAD:
		printf("  LOAD from <%u> to <%u>\r\n", query->copy.from, query->copy.to);
		break;
	case RPARAM_CLEAR:
		printf("  CLEAR <%u>\r\n", query->clear.id);
		break;
	case RPARAM_SAVE:
		printf("  SAVE from <%u> to <%u>\r\n", query->copy.from, query->copy.to);
		break;
	default:
		printf("Unknown rparm action\r\n");
		break;
	}
	return CMD_ERROR_NONE;
}

int cmd_rparam_copy(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	int from = atoi(ctx->argv[1]);
	int to = atoi(ctx->argv[2]);

	query->action = RPARAM_COPY;
	query->copy.from = from;
	query->copy.to = to;
	query->length = 2;

	cmd_rparam_print(NULL);

	return CMD_ERROR_NONE;
}

int cmd_rparam_load(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	int from = atoi(ctx->argv[1]);
	int to = atoi(ctx->argv[2]);

	query->action = RPARAM_LOAD;
	query->copy.from = from;
	query->copy.to = to;
	query->length = 2;

	cmd_rparam_print(NULL);

	return CMD_ERROR_NONE;
}

int cmd_rparam_save(struct command_context *ctx) {
	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	int from = atoi(ctx->argv[1]);
	int to = atoi(ctx->argv[2]);

	query->action = RPARAM_SAVE;
	query->copy.from = from;
	query->copy.to = to;
	query->length = 2;

	cmd_rparam_print(NULL);

	return CMD_ERROR_NONE;
}

int cmd_rparam_clear(struct command_context *ctx) {
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	int file_id = atoi(ctx->argv[1]);

	query->action = RPARAM_CLEAR;
	query->clear.id = file_id;
	query->length = 1;

	cmd_rparam_print(NULL);

	return CMD_ERROR_NONE;
}


int cmd_rparam_reset(struct command_context *ctx) {
	query->action = RPARAM_GET;
	query->length = 0;
	query_get_size = 0;
	return CMD_ERROR_NONE;
}

command_t rparam_commands[] = {
	{
		.name = "init",
		.help = "Setup table, node and port",
		.usage = "<node> [mem]",
		.handler = cmd_rparam_init,
	},{
		.name = "download",
		.help = "Download table",
		.usage = "<node> [mem]",
		.handler = cmd_rparam_download,
	},{
		.name = "list",
		.help = "Table list",
		.handler = cmd_rparam_list,
	},{
		.name = "get",
		.help = "Query get",
		.usage = "<name>",
		.handler = cmd_rparam_get,
	},{
		.name = "set",
		.help = "Query set",
		.usage = "<name> <value>",
		.handler = cmd_rparam_set,
	},{
		.name = "copy",
		.usage = "<from> <to>",
		.help = "Query copy",
		.handler = cmd_rparam_copy,
	},{
		.name = "load",
		.usage = "<from> <to>",
		.help = "Load from fs/fram to mem",
		.handler = cmd_rparam_load,
	},{
		.name = "save",
		.usage = "<from> <to>",
		.help = "Save from mem to fs/fram",
		.handler = cmd_rparam_save,
	},{
		.name = "clear",
		.usage = "<file_id>",
		.help = "Clear data from fs/fram",
		.handler = cmd_rparam_clear,
	},{
		.name = "print",
		.help = "Query print",
		.handler = cmd_rparam_print,
	},{
		.name = "reset",
		.help = "Query reset",
		.handler = cmd_rparam_reset,
	},{
		.name = "send",
		.usage = "[crc override]",
		.help = "Query send",
		.handler = cmd_rparam_send,
	},{
		.name = "autosend",
		.usage = "<bool>",
		.help = "set or unset autosend mode",
		.handler = cmd_rparam_set_autosend,
	},{
		.name = "getall",
		.help = "Download full table contents",
		.handler = cmd_rparam_getall,
	}
};

command_t __root_command rparam_root_command[] = {
	{
		.name = "rparam",
		.help = "Remote Parameter System",
		.chain = INIT_CHAIN(rparam_commands),
	},
};

void cmd_rparam_setup(void) {
	command_register(rparam_root_command);
}
