/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_STORE_H_
#define PARAM_STORE_H_

/**
 * Storage backend prototypes
 */
int param_save(param_index_t * from, uint8_t to_file_id);
int param_load(uint8_t from_file_id, param_index_t * to);
void param_clear(uint8_t file_id);
void param_write_persist(uint16_t addr, uint16_t framaddr, size_t size, void * item, uint8_t flags);
void param_init_persist(param_index_t * mem);

int param_save_file(const param_table_t table[], size_t table_count, param_mem from, char * to);
int param_load_file(const param_table_t table[], size_t table_count, char * from, param_mem to);

/**
 * Load parameters with fallback function
 * @param mem pointer to memory to load
 * @param fno first try this file
 * @param fno_dfl then try this file
 * @param fallback finally call fallback
 * @param name pointer to config name
 * @returns param_config_id 0 = fallback, 1 = default, 2 = stored
 */
param_config_id param_load_fallback(param_index_t * mem, uint8_t fno, uint8_t fno_dfl, void (*fallback)(void), char * name);

#endif /* PARAM_STORE_H_ */
