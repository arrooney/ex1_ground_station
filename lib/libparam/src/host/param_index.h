/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_INDEX_H_
#define PARAM_INDEX_H_

#include <conf_param.h>
#include <param_lock.h>

extern param_index_t param_index[];

static inline param_index_t * param_ptr(param_mem mem) {
	if (mem < PARAM_MAX_TABLES)
		return &param_index[mem];
	return NULL;
}

static inline void param_index_set(param_mem mem, param_index_t table) {
	if (mem >= PARAM_MAX_TABLES)
		return;
	param_index[mem] = table;
	param_index[mem].mem_id = mem;
	param_lock_init(param_ptr(mem));
}

/**
 * Get checksum of parameter table
 * @param mem pointer to table
 * @return 16-bit fletcher checksum
 */
uint16_t param_index_chksum(param_index_t * mem);

#endif /* PARAM_INDEX_H_ */
