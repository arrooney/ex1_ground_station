/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <util/log.h>

#include <param_lock.h>
#include <param/param_types.h>
#include <param/param_memio.h>

const param_table_t * param_find_name(const param_table_t table[], size_t table_count, char * name) {
	if (table == NULL)
		return NULL;

	for (unsigned int i = 0; i < table_count; i++) {
	  if (param_strncmp(name, table[i].name, MAX_PARAM_NAME_LEN) == 0)
			return &table[i];
	}
	return NULL;
}

const param_table_t * param_find_addr(const param_table_t table[], size_t table_count, uint16_t addr) {
	if (table == NULL)
		return NULL;
	for (unsigned int i = 0; i < table_count; i++) {
		const param_table_t * param = &table[i];

		int count = PARAM_COUNT;
		if (count == 0)
			count = 1;

		for (int j = 0; j < count; j++) {
			if (PARAM_ADDR + (j * PARAM_SIZE) == addr)
				return &table[i];
		}
	}
	return NULL;
}

int param_to_string(const param_table_t * param, uint8_t * buf, int pos, void * value, int with_type, int max_size) {

	int size = 0;
	switch (PARAM_TYPE) {
		case PARAM_UINT8: {
			if (with_type) size += sprintf((char *) &buf[pos + size], "U8  ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRIu8, *(uint8_t *) value);
			break;
		}
		case PARAM_UINT16: {
			uint16_t tmp;
			memcpy(&tmp, value, sizeof(uint16_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "U16 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRIu16, tmp);
			break;
		}
		case PARAM_UINT32: {
			uint32_t tmp;
			memcpy(&tmp, value, sizeof(uint32_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "U32 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRIu32, tmp);
			break;
		}
#ifdef PRIu64
		case PARAM_UINT64: {
			uint64_t tmp;
			memcpy(&tmp, value, sizeof(uint64_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "U64 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRIu64, tmp);
			break;
		}
#endif
		case PARAM_INT8: {
			if (with_type) size += sprintf((char *) &buf[pos + size], "I8  ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRId8, *(int8_t *) value);
			break;
		}
		case PARAM_INT16: {
			int16_t tmp;
			memcpy(&tmp, value, sizeof(int16_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "I16 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRId16, tmp);
			break;
		}
		case PARAM_INT32: {
			int32_t tmp;
			memcpy(&tmp, value, sizeof(int32_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "I32 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRId32, tmp);
			break;
		}
#ifdef PRId64
		case PARAM_INT64: {
			int64_t tmp;
			memcpy(&tmp, value, sizeof(int64_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "I64 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRId64, tmp);
			break;
		}
#endif
		case PARAM_X8: {
			if (with_type) size += sprintf((char *) &buf[pos + size], "X8  ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "0x%02"PRIX8, *(uint8_t *) value);
			break;
		}
		case PARAM_X16: {
			uint16_t tmp;
			memcpy(&tmp, value, sizeof(uint16_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "X16 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "0x%04"PRIX16, tmp);
			break;
		}
		case PARAM_X32: {
			uint32_t tmp;
			memcpy(&tmp, value, sizeof(uint32_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "X32 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "0x%08"PRIX32, tmp);
			break;
		}
#ifdef PRIX64
		case PARAM_X64: {
			uint64_t tmp;
			memcpy(&tmp, value, sizeof(uint64_t));
			if (with_type) size += sprintf((char *) &buf[pos + size], "X64 ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "0x%016"PRIX64, tmp);
			break;
		}
#endif
		case PARAM_FLOAT: {
			float tmp;
			memcpy(&tmp, value, sizeof(float));
			if (with_type) size += sprintf((char *) &buf[pos + size], "FLT ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%g", (double) tmp);
			break;
		}
		case PARAM_DOUBLE: {
			double tmp;
			memcpy(&tmp, value, sizeof(double));
			if (with_type) size += sprintf((char *) &buf[pos + size], "DBL ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "%g", (double) tmp);
			break;
		}
		case PARAM_STRING: {
			if (with_type) size += sprintf((char *) &buf[pos + size], "STR ");
			size += snprintf((char *) &buf[pos + size], max_size - size, "\"%s\"", (char *) value);
			break;
		}
		case PARAM_DATA: {
			if (with_type) size += sprintf((char *) &buf[pos + size], "DAT ");
			for (int i = 0; i < PARAM_SIZE; i++)
				size += snprintf((char *) &buf[pos + size], max_size - size, "%"PRIX8, ((uint8_t *) value)[i]);
			break;
		}
		default: {
			printf("PARAM Unknown param type %u", PARAM_TYPE);
			break;
		}
	}

	return size;
}

int param_from_string(const param_table_t * param, char * string, void * value) {

	switch(PARAM_TYPE) {

		case PARAM_UINT8: {
			/* NOTE: We use a 16-bit temp variable because avrlibc does not support hhx types */
			uint16_t parsein;
			sscanf(string, "%"SCNu16, &parsein);
			*(uint8_t *) value = parsein;
			break;
		}
		case PARAM_UINT16: {
			uint16_t parsein;
			sscanf(string, "%"SCNu16, &parsein);
			*(uint16_t *) value = parsein;
			break;
		}
		case PARAM_UINT32: {
			uint32_t parsein;
			sscanf(string, "%"SCNu32, &parsein);
			*(uint32_t *) value = parsein;
			break;
		}
#ifdef SCNu64
		case PARAM_UINT64: {
			uint64_t parsein;
			sscanf(string, "%"SCNu64, &parsein);
			*(uint64_t *) value = parsein;
			break;
		}
#endif

		case PARAM_INT8: {
			/* NOTE: We use a 16-bit temp variable because avrlibc does not support hhx types */
			int16_t parsein;
			sscanf(string, "%"SCNd16, &parsein);
			*(int8_t *) value = parsein;
			break;
		}
		case PARAM_INT16: {
			int16_t parsein;
			sscanf(string, "%"SCNd16, &parsein);
			*(int16_t *) value = parsein;
			break;
		}
		case PARAM_INT32: {
			int32_t parsein;
			sscanf(string, "%"SCNd32, &parsein);
			*(int32_t *) value = parsein;
			break;
		}
#ifdef SCNd64
		case PARAM_INT64: {
			int64_t parsein;
			sscanf(string, "%"SCNd64, &parsein);
			*(int64_t *) value = (int64_t) parsein;
			break;
		}
#endif

		case PARAM_X8: {
			/* NOTE: We use a 16-bit temp variable because avrlibc does not support hhx types */
			int16_t parsein;
			sscanf(string, "%"SCNx16, &parsein);
			*(int8_t *) value = parsein;
			break;
		}
		case PARAM_X16: {
			int16_t parsein;
			sscanf(string, "%"SCNx16, &parsein);
			*(int16_t *) value = parsein;
			break;
		}
		case PARAM_X32: {
			int32_t parsein;
			sscanf(string, "%"SCNx32, &parsein);
			*(int32_t *) value = parsein;
			break;
		}
#ifdef SCNx64
		case PARAM_X64: {
			uint64_t parsein;
			sscanf(string, "%"SCNx64, &parsein);
			*(uint64_t *) value = parsein;
			break;
		}
#endif

		case PARAM_FLOAT:
		{
			double parsein;
			sscanf(string, "%lf", &parsein);
			*(float *) value = (float) parsein;
			break;
		}

		case PARAM_DOUBLE: {
			double parsein;
			sscanf(string, "%lf", &parsein);
			*(double *) value = parsein;
			break;
		}

		case PARAM_STRING: {
			memcpy(value, string, PARAM_SIZE);
			break;
		}

		case PARAM_DATA: {
			int len = strlen(string) / 2;
			char * out = (char *) value;
			memset(out, 0, PARAM_SIZE);

			unsigned int to_int(char c) {
				if (c >= '0' && c <= '9') return      c - '0';
				if (c >= 'A' && c <= 'F') return 10 + c - 'A';
				if (c >= 'a' && c <= 'f') return 10 + c - 'a';
				return -1;
			}

			for (int i = 0; ((i < len) && (i < PARAM_SIZE)); ++i)
				out[i] = 16 * to_int(string[2*i]) + to_int(string[2*i+1]);

			break;
		}

		default: {
			return -1;
			break;
		}

	}

	return 0;

}

void param_list_single(const param_table_t * param, param_index_t * mem, int do_read) {
	if (param == NULL)
		return;
	printf("  0x%04X ", PARAM_ADDR);
	char buf[MAX_PARAM_NAME_LEN + 1] = {};
	param_memcpy(buf, param->name, MAX_PARAM_NAME_LEN);
	printf("%-16s\t", buf);
	if (do_read) {
		int count = PARAM_COUNT;
		if (count == 0)
			count = 1;

		for (int j = 0; j < count; j++) {
			uint8_t string[100];
			param_lock(mem);
			param_to_string(param, string, 0, param_read_addr(PARAM_ADDR + PARAM_SIZE * j, mem, PARAM_SIZE), (j == 0) ? 1 : 0, 100);
			param_unlock(mem);
			printf("%s ", string);
		}
	}
	printf("\r\n");
}

void param_list(param_index_t * mem, int do_read) {
	if (mem == NULL)
		return;
	for (unsigned int i = 0; i < mem->count; i++)
		param_list_single(&mem->table[i], mem, do_read);
}
