/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_MEMIO_H_
#define PARAM_MEMIO_H_

#include <string.h>

#include <conf_param.h>
#include <param/param_types.h>
#include <param_store.h>
#include <param_lock.h>

/**
 * Convert from logical parameter address into physical mem address
 * This also reads in data from an external device and returns a pointer to it.
 * @param addr Logical parameter address
 * @param mem Pointer to indexed parameter table
 * @param size Size of data to read
 * @return Pointer to memory
 */
static inline void * param_read_addr(uint16_t addr, param_index_t * mem, size_t size) {

	/* Check if read is out of bounds */
	if (addr + size > mem->size)
		return NULL;

	/* Try to read from RAM */
	if (mem->physaddr != NULL) {
		return mem->physaddr + addr;

	/* Try to read from external read() function */
	} else if (mem->read != NULL) {
		return (*mem->read)(addr, size);
	}

	/* Otherwise, give up */
	return NULL;
}

/**
 * Write a parameter into physical memory using a logical address
 * @param addr Logical address of parameter to write
 * @param mem Pointer to indexed parameter table to use
 * @param size Size of parameter to write
 * @param item Pointer to where data should be copied from
 * @param flags Write flags
 */
static inline void param_write_addr(uint16_t addr, param_index_t * mem, size_t size, void * item, uint8_t flags) {

	/* Check if write is out of bounds */
	if (addr + size > mem->size)
		return;

	/* Try writing to RAM */
	if (mem->physaddr != NULL) {
		param_lock(mem);
		memcpy(mem->physaddr + addr, item, size);
		if (flags & PARAM_F_PERSIST && mem->framaddr != 0) {
			param_write_persist(addr, mem->framaddr, size, item, flags);
		}
		param_unlock(mem);

	/* Otherwise, try write using write() function */
	} else if (mem->write != NULL) {
		(*mem->write)(addr, size, item, flags);
	}

	/* Callback here */
	if (mem->callback != NULL && (flags & PARAM_F_NOCALLBACK) == 0)
		(*mem->callback)(addr, mem);

}

#endif /* PARAM_MEMIO_H_ */
