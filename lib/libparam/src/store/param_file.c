/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>

#include <command/command.h>
#include <param/param.h>

int param_save_file(const param_table_t table[], size_t table_count, param_mem from, char * to) {

	FILE * fd = fopen(to, "w+");
	if (fd == NULL) {
		return -1;
	}

	fprintf(fd, "param mem %u\r\n", from);

	for (int i = 0; i < table_count; i++) {
	  fprintf(fd, "param set %.*s", MAX_PARAM_NAME_LEN, table[i].name);

		int count = table[i].count;
		if (count == 0)
			count = 1;

		for (int j = 0; j < count; j++) {
			uint8_t string[100];
			param_to_string(&table[i], string, 0, param_read_addr(table[i].addr + table[i].size * j, param_ptr(from), table[i].size), 0,100);
			fprintf(fd, " %s", string);
		}

		fprintf(fd, "\r\n");

	}
	fflush(fd);
	fclose(fd);

	return 0;

}

int param_load_file(const param_table_t table[], size_t table_count, char * from, param_mem to) {

	FILE * fd = fopen(from, "r");
	if (fd == NULL) {
		return -1;
	}

	/* Read from file */
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	extern param_mem cmd_param_mem;
	cmd_param_mem = to;

	while ((read = getline(&line, &len, fd)) != -1) {
		command_run(line);
	}

	if (line)
	   free(line);

	fclose(fd);

	return 0;
}

void param_write_persist(uint16_t addr, uint16_t framaddr, size_t size, void * item, uint8_t flags) {
	return;
};

int param_load(uint8_t from_file_id, param_index_t * to) {
	char fname[20];
	snprintf(fname, 19, "store-%u.txt", from_file_id);
	return param_load_file(to->table, to->count, fname, to->mem_id);
};

int param_save(param_index_t * from, uint8_t to_file_id) {
	char fname[20];
	snprintf(fname, 19, "store-%u.txt", to_file_id);
	return param_save_file(from->table, from->count, from->mem_id, fname);
};

void param_clear(uint8_t file_id) {
	return;
}
