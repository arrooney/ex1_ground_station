/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <util/log.h>
#include <param/param_types.h>
#include <param/param_memio.h>
#include <param/param_string.h>
#include "param_index.h"

void param_copy(param_index_t * from, param_index_t * to, unsigned int to_offset) {

	/* Check valid destination */
	if (from == NULL || to == NULL)
		return;

	/* Check if destination is scratch memory */
	if (to->scratch_size > 0 && from->size <= to->scratch_size) {
		to->table = from->table;
		to->count = from->count;
		to->size = from->size;
		to->table_chksum = 0;
		printf("Set up scratch copy of %u in table %u\r\n", from->mem_id, to->mem_id);
	}

	/* Move in RAM */
	if ((from->table == to->table) || (to_offset > 0)) {

		for (unsigned int i = 0; i < from->count; i++) {
			const param_table_t * param = &from->table[i];
			int count = PARAM_COUNT;

			if (count == 0)
				count = 1;

			for (int j = 0; j < count; j++) {
				uint16_t addr = PARAM_ADDR + (PARAM_SIZE * j);
				void * item = param_read_addr(addr, from, PARAM_SIZE);
				param_write_addr(addr + to_offset, to, PARAM_SIZE, item, PARAM_FLAGS);
			}
		}

	}

}

void param_set_string(uint16_t addr, param_mem mem_id, char * value) {
	param_index_t * index = param_ptr(mem_id);
	if (index == NULL)
		return;
	const param_table_t * param = param_find_addr(index->table, index->count, addr);
	if (param == NULL)
		return;
	param_write_addr(addr, index, PARAM_SIZE, value, PARAM_FLAGS);
}

char * param_get_string(uint16_t addr, param_mem mem_id) {
	param_index_t * index = param_ptr(mem_id);
	if (index == NULL)
		return NULL;
	const param_table_t * param = param_find_addr(index->table, index->count, addr);
	if (param == NULL)
		return NULL;
	return param_read_addr(addr, index, PARAM_SIZE);
}
