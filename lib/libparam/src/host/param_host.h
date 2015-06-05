/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_HOST_H_
#define PARAM_HOST_H_

#include <stdint.h>
#include <param/param_types.h>
#include <param/param_memio.h>
#include <param/param_string.h>
#include <param_index.h>

#include <conf_param.h>
#include <param_lock.h>

void param_copy(param_index_t * from, param_index_t * to, unsigned int to_offset);
void cmd_param_setup();

#define PARAM_PASTE(name, type)                                                                 \
static inline type param_get_##name(uint16_t addr, param_mem mem_id) {                          \
	param_index_t * mem = param_ptr(mem_id);                                                    \
	param_lock(mem);	                                                                        \
	type ret = 0, *ptr = param_read_addr(addr, mem, sizeof(type));                              \
	if (ptr != NULL)                                                                            \
		ret = *ptr;                                                                             \
	param_unlock(mem);                                                                          \
	return ret;                                                                                 \
}                                                                                               \
static inline type param_get_##name##_fromidx(uint16_t addr, param_index_t * mem) {             \
	param_lock(mem);                                                                            \
	type ret = 0, *ptr = param_read_addr(addr, mem, sizeof(type));                              \
	if (ptr != NULL)                                                                            \
		ret = *ptr;                                                                             \
	param_unlock(mem);                                                                          \
	return ret;                                                                                 \
}                                                                                               \
static inline type* param_get_ptr_##name(uint16_t addr, param_mem mem_id) {						\
	param_index_t * mem = param_ptr(mem_id);													\
	type* ptr = (type *) param_read_addr(addr, mem, sizeof(type));								\
	return ptr;																					\
}																								\
static inline void param_set_##name(uint16_t addr, param_mem mem_id, type value) {              \
	param_write_addr(addr, param_ptr(mem_id), sizeof(type), &value, 0);                         \
}                                                                                               \
static inline void param_set_##name##_persist(uint16_t addr, param_mem mem_id, type value) {    \
	param_write_addr(addr, param_ptr(mem_id), sizeof(type), &value, PARAM_F_PERSIST);           \
}                                                                                               \
static inline void param_set_##name##_nocallback(uint16_t addr, param_mem mem_id, type value) { \
	param_write_addr(addr, param_ptr(mem_id), sizeof(type), &value, PARAM_F_NOCALLBACK);        \
}                                                                                               \

PARAM_PASTE(uint8, uint8_t);
PARAM_PASTE(uint16, uint16_t);
PARAM_PASTE(uint32, uint32_t);
PARAM_PASTE(uint64, uint64_t);
PARAM_PASTE(int8, int8_t);
PARAM_PASTE(int16, int16_t);
PARAM_PASTE(int32, int32_t);
PARAM_PASTE(int64, int64_t);
PARAM_PASTE(double, double);
PARAM_PASTE(float, float);

void param_set_string(uint16_t addr, param_mem mem_id, char * value);
char * param_get_string(uint16_t addr, param_mem mem_id);

#endif /* PARAM_HOST_H_ */
