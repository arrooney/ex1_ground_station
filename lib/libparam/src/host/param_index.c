/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <conf_param.h>
#include <param/param_types.h>
#include <param/param_fletcher.h>

/* Global storage of parameter index */
param_index_t param_index[PARAM_MAX_TABLES] = {};

uint16_t param_index_chksum(param_index_t * mem) {

	if (mem->table_chksum != 0)
		return mem->table_chksum;

#ifdef __AVR__
	mem->table_chksum = fletcher16_P((void *) mem->table, mem->count * sizeof(param_table_t));
#else
	mem->table_chksum = fletcher16((void *) mem->table, mem->count * sizeof(param_table_t));
#endif

	return mem->table_chksum;
}
