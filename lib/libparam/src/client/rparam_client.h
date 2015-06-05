/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef RPARAM_CLIENT_H_
#define RPARAM_CLIENT_H_

#include <inttypes.h>
#include <param/param_types.h>

void cmd_rparam_setup(void);
int rparam_get_full_table(param_index_t * mem, int node, int port, int mem_id, int timeout);
int rparam_get_single(void * out, uint16_t addr, param_type_t type, int size, int mem_id, int node, int port, int timeout);

#if 0
#define RPARAM_PASTE(name, type, rparamtype)                                                                       \
static inline type rparam_get_##name(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {     \
	return rparam_get_single(out, addr, rparamtype, sizeof(type), mem_id, node, port, timeout);                    \
}                                                                                                                  \

RPARAM_PASTE(uint8, uint8_t, PARAM_UINT8)
RPARAM_PASTE(uint16, uint16_t, PARAM_UINT16)
RPARAM_PASTE(uint32, uint32_t, PARAM_UINT32)
RPARAM_PASTE(uint64, uint64_t, PARAM_UINT64)
RPARAM_PASTE(int8, int8_t, PARAM_INT8)
RPARAM_PASTE(int16, int16_t, PARAM_INT16)
RPARAM_PASTE(int32, int32_t, PARAM_INT32)
RPARAM_PASTE(int64, int64_t, PARAM_INT64)
RPARAM_PASTE(float, float, PARAM_FLOAT)
RPARAM_PASTE(double, double, PARAM_DOUBLE)
#else
uint8_t rparam_get_uint8(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
uint16_t rparam_get_uint16(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
uint32_t rparam_get_uint32(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
uint64_t rparam_get_uint64(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
int8_t rparam_get_int8(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
int16_t rparam_get_int16(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
int32_t rparam_get_int32(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
int64_t rparam_get_int64(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
float rparam_get_float(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
double rparam_get_double(void * out, uint16_t addr, int mem_id, int node, int port, int timeout);
#endif

#endif /* RPARAM_CLIENT_H_ */
