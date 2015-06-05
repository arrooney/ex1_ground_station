#include <param/param_serializer.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <alloca.h>

#include <util/log.h>

#include <conf_param.h>
#include <param/param_string.h>
#include <param/param_types.h>
#include <param/param_memio.h>

#if ENABLE_PARAM_LOCAL_CLIENT
#include "host/param_index.h"
#endif

#include <csp/csp_endian.h>

int param_betoh(param_type_t type, void * item) {
	switch (type) {
	case PARAM_UINT16:
	case PARAM_INT16:
	case PARAM_X16:
	{
		*(uint16_t *) item = csp_betoh16(*(uint16_t *) item);
		return 1;
	}
	case PARAM_UINT32:
	case PARAM_INT32:
	case PARAM_X32:
	{
		*(uint32_t *) item = csp_betoh32(*(uint32_t *) item);
		return 1;
	}
	case PARAM_UINT64:
	case PARAM_INT64:
	case PARAM_X64:
	{
		*(uint64_t *) item = csp_betoh64(*(uint64_t *) item);
		return 1;
	}
	case PARAM_FLOAT:
	{
		*(float *) item = csp_ntohflt(*(float *) item);
		return 1;
	}
	case PARAM_DOUBLE:
	{
		*(double *) item = csp_ntohdbl(*(double *) item);
		return 1;
	}
	default:
		return 0;
	}
}

int param_htobe(param_type_t type, void * item) {
	switch (type) {
	case PARAM_UINT16:
	case PARAM_INT16:
	case PARAM_X16:
	{
		*(uint16_t *) item = csp_htobe16(*(uint16_t *) item);
		return 1;
	}
	case PARAM_UINT32:
	case PARAM_INT32:
	case PARAM_X32:
	{
		*(uint32_t *) item = csp_htobe32(*(uint32_t *) item);
		return 1;
	}
	case PARAM_UINT64:
	case PARAM_INT64:
	case PARAM_X64:
	{
		*(uint64_t *) item = csp_htobe64(*(uint64_t *) item);
		return 1;
	}
	case PARAM_FLOAT:
	{
		*(float *) item = csp_htonflt(*(float *) item);
		return 1;
	}
	case PARAM_DOUBLE:
	{
		*(double *) item = csp_htondbl(*(double *) item);
		return 1;
	}
	default:
		return 0;
	}
}

int param_serialize_array(param_index_t * mem, const param_table_t * param, uint8_t * buf, uint16_t * pos, unsigned int maxlen, param_serializer_flags flags) {

	/* Get count */
	int count = PARAM_COUNT;
	if (count == 0)
		count = 1;

	/* Calculate total parameter size (full array) */
	unsigned int size = PARAM_SIZE * count;
	if ((flags & F_PACKED) == 0)
		size += count * sizeof(uint16_t);

	/* Return if parameter array would exceed maxbuf */
	if (*pos + size > maxlen)
		return -1;

	for (int j = 0; j < count; j++) {
		uint16_t work_addr = PARAM_ADDR + (PARAM_SIZE * j);
		void * item = param_read_addr(work_addr, mem, PARAM_SIZE);
		if (param_serialize_item(param, work_addr, buf, pos, maxlen, item, flags) < 0) {
			return -1;
		}
	}

	return 0;

}

int param_serialize_item(const param_table_t * param, uint16_t addr, uint8_t * buf, uint16_t * pos, unsigned int maxlen, void * item, param_serializer_flags flags) {

	/* Check length */
	if (((flags & F_PACKED) ? 0 : sizeof(uint16_t)) + PARAM_SIZE + *pos > maxlen)
		return -1;

	/* Do helpfull printout if requested */
	if (flags & F_PRINT_GET) {
		printf("  GET ");
		PARAM_PRINT_NAME();
		printf(" 0x%X [%u]\r\n", PARAM_ADDR, PARAM_SIZE);
	}

	/* Address */
	if (flags & F_TO_BIG_ENDIAN)
		addr = csp_htobe16(addr);

	/* Include address if not packed */
	if ((flags & F_PACKED) == 0) {
		if ((flags & F_DRY_RUN) == 0)
			memcpy(&buf[*pos], &addr, sizeof(uint16_t));
		*pos += sizeof(uint16_t);
	}

	if (flags & F_TO_BIG_ENDIAN) {

		/* Do not try conversion on strings */
		if (PARAM_TYPE != PARAM_DATA && PARAM_TYPE != PARAM_STRING) {

			/* Generate temporary stack space for the converted value:
			 * The memory we are serializing may not be touched, so we need a temporary variable of variable length */
			void * tmp = alloca(PARAM_SIZE);
			memcpy(tmp, item, PARAM_SIZE);

			/* Convert endians - inplace - on the tmp variable */
			param_htobe(PARAM_TYPE, tmp);

			/* Overwrite the pointer to the data, with the tmp pointer */
			item = tmp;

		}

	}

	if ((flags & F_DRY_RUN) == 0)
		memcpy(&buf[*pos], item, PARAM_SIZE);
	*pos += PARAM_SIZE;
	return sizeof(uint16_t) + PARAM_SIZE;

}

int param_serialize_full_table(param_index_t * mem, unsigned int *start, uint8_t * buf, unsigned int maxbuflen, param_serializer_flags flags) {
	uint16_t pos = 0;
	for (unsigned int i = *start; i < mem->count; i++, (*start)++) {
		const param_table_t * param = &mem->table[i];
		param_lock(mem);
		if (param_serialize_array(mem, param, buf, &pos, maxbuflen, flags) < 0) {
			param_unlock(mem);
			break;
		}
		param_unlock(mem);
	}
	return pos;
}

int param_serialize_list(param_index_t * mem, const uint16_t addr[], unsigned int addr_count, unsigned int *start, uint8_t * buf, unsigned int maxbuflen, param_serializer_flags flags) {
	uint16_t pos = 0;

	for (unsigned int i = *start; i < addr_count; i++, (*start)++) {
		if (mem == NULL)
			continue;

		const param_table_t * param = param_find_addr(mem->table, mem->count, addr[i]);
		if (param == NULL)
			continue;

		if (param_serialize_item(param, addr[i], buf, &pos, maxbuflen, param_read_addr(addr[i], mem, PARAM_SIZE), flags) < 0)
			break;
	}
	if ((*start) == addr_count) {
		/* Set start to indicate we're done */
		(*start) = mem->count;
	}
	return pos;
}

int param_deserialize_item(const param_table_t * param, uint16_t addr, param_index_t * mem, void * item, param_serializer_flags flags) {

	if (flags & F_FROM_BIG_ENDIAN) {

		/* Do not try conversion on strings */
		if (PARAM_TYPE != PARAM_DATA && PARAM_TYPE != PARAM_STRING) {

			/* Generate temporary stack space for the converted value:
			 * The memory we are deserializing may not be touched, so we need a temporary variable of variable length */
			void * tmp = alloca(PARAM_SIZE);
			memcpy(tmp, item, PARAM_SIZE);

			/* Convert endians - inplace - on the tmp variable */
			param_betoh(PARAM_TYPE, tmp);

			/* Overwrite the pointer to the data, with the tmp pointer */
			item = tmp;
		}
	}

	if (flags & (F_PRINT_REP | F_PRINT_SET)) {

		/* Stringify */
		uint8_t buf[100] = {};
		param_to_string(param, buf, 0, item, 0, 100);

		if (flags & F_PRINT_REP) {
			printf("  REP ");
		} else if (flags & F_PRINT_SET) {
			printf("  SET ");
		}

		PARAM_PRINT_NAME();
		if (PARAM_COUNT > 0) {
			int array_offset = (addr - PARAM_ADDR) / PARAM_SIZE;
			printf("[%u] = %s (%u)\r\n", array_offset, buf, PARAM_SIZE);
		} else {
		  printf(" = %s (%u)\r\n", buf, PARAM_SIZE);
		}

	}

	if ((flags & F_DRY_RUN) == 0)
		param_write_addr(addr, mem, PARAM_SIZE, item, PARAM_FLAGS);

	return 0;
}

int param_deserialize(param_index_t * mem, uint8_t * buf, int maxlen, param_serializer_flags flags) {
	int pos = 0, count = 0;
	while(pos < maxlen) {

		uint16_t addr = 0;
		const param_table_t * param = NULL;

		/** PACKED */
		if (flags & F_PACKED) {

			/* Find in table */
			param = &mem->table[count];

			/* Get count */
			int count = PARAM_COUNT;
			if (count == 0)
				count = 1;

			/* For each item in array */
			for (int j = 0; j < count; j++) {
				addr = PARAM_ADDR + (PARAM_SIZE * j);
				param_deserialize_item(param, addr, mem, &buf[pos], flags);
				pos += PARAM_SIZE;
			}

		/** NOT PACKED */
		} else {

			/* Read address from data */
			memcpy(&addr, &buf[pos], sizeof(uint16_t));
			if (flags & F_FROM_BIG_ENDIAN)
				addr = csp_betoh16(addr);
			pos += sizeof(uint16_t);

			/* Find in table */
			param = param_find_addr(mem->table, mem->count, addr);
			if (param == NULL) {
				//printf("Param error could not find addr %u\r\n", addr);
				return -1;
			}

			/* Copy value */
			param_deserialize_item(param, addr, mem, &buf[pos], flags);
			pos += PARAM_SIZE;

		}

		count++;

	}
	return 0;
}
