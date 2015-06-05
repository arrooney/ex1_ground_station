/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <param/param_types.h>

int param_save(param_index_t * from, uint8_t to_file_id) {
	return 0;
}

int param_load(uint8_t file_id, param_index_t * to) {
	return -1;
}

void param_clear(uint8_t file_id) {
	return;
}

void param_write_persist(uint16_t addr, uint16_t framaddr, size_t size, void * item, uint8_t flags) {
	return;
}

void param_init_persist(param_index_t * mem) {
	return;
}
