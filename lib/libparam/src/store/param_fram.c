/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <conf_param.h>
#include <util/log.h>
#include <param/param_types.h>
#include <param/param_fletcher.h>
#include <param_host.h>

/**
 * Extern prototypes
 * We put them here to avoid issues and dependencies with include directives.
 */
extern void fm33256b_write_data(uint16_t addr, uint8_t *data, uint16_t len);
extern void fm33256b_read_data(uint16_t addr, uint8_t *data, uint16_t len);

/* memcpy compatible wrapper around read_data */
static inline void * fm33256b_memcpy_read(void * buf, const void * addr, size_t len) {
	fm33256b_read_data((uint16_t) (uintptr_t) addr, (uint8_t *) buf, (uint16_t) len);
	return buf;
}

int param_save(param_index_t * from, uint8_t file_id) {

	/* Check valid source */
	if (from->table == NULL)
		return -1;

	/* Check valid size */
	if (from->size + 4 > PARAM_MAX_FILESIZE)
		return -1;

	/* Calculate physical address in FRAM */
	int phybegin = PARAM_MAX_FILESIZE * file_id;
	int crc_table = phybegin + PARAM_MAX_FILESIZE - sizeof(uint16_t);
	int crc_data = crc_table - sizeof(uint16_t);

	/* write table CRC */
	uint16_t crc = param_index_chksum(from);
	printf("Table CRC %u\r\n", crc);
	fm33256b_write_data(crc_table, (void *) &crc, sizeof(uint16_t));

	for (unsigned int i = 0; i < from->count; i++) {
		const param_table_t * param = &from->table[i];
		int count = PARAM_COUNT;
		if (count == 0)
			count = 1;
		for (int j = 0; j < count; j++) {
			void * fromaddr = param_read_addr(PARAM_ADDR + PARAM_SIZE * j, from, PARAM_SIZE);
			uint16_t toaddr = phybegin + PARAM_ADDR +  PARAM_SIZE * j;
			fm33256b_write_data(toaddr, fromaddr, PARAM_SIZE);
		}
	}

	/* write data CRC */
	crc = fletcher16_memcpy((void *) phybegin, from->size, fm33256b_memcpy_read);
	fm33256b_write_data(crc_data, (void *) &crc, sizeof(uint16_t));
	printf("Data CRC %u\r\n", crc);

	return 0;
}

int param_load(uint8_t file_id, param_index_t * to) {

	/* Check valid destination */
	if (to->table == NULL)
		return -1;

	/* Calculate physical address in FRAM */
	int phybegin = PARAM_MAX_FILESIZE * file_id;
	int crc_table = phybegin + PARAM_MAX_FILESIZE - sizeof(uint16_t);
	int crc_data = crc_table - sizeof(uint16_t);

	/* Read table CRC */
	uint16_t crc = param_index_chksum(to);
	uint16_t stored_crc;
	fm33256b_read_data(crc_table, (void *) &stored_crc, sizeof(uint16_t));
	if (crc != stored_crc) {
		//printf("Invalid table crc %u != %u\r\n", crc, stored_crc);
		return -1;
	}

	/* Read data CRC */
	crc = fletcher16_memcpy((void *) phybegin, to->size, fm33256b_memcpy_read);
	fm33256b_read_data(crc_data, (void *) &stored_crc, sizeof(uint16_t));
	if (crc != stored_crc) {
		//printf("Invalid data crc %u != %u\r\n", crc, stored_crc);
		return -1;
	}

	for (unsigned int i = 0; i < to->count; i++) {
		const param_table_t * param = &to->table[i];
		int count = PARAM_COUNT;
		if (count == 0)
			count = 1;
		for (int j = 0; j < count; j++) {
			uint16_t fromaddr = phybegin + PARAM_ADDR + PARAM_SIZE * j;
			void * toaddr = param_read_addr(PARAM_ADDR + PARAM_SIZE * j, to, PARAM_SIZE);
			fm33256b_read_data(fromaddr, toaddr, PARAM_SIZE);
		}
	}

	return 0;
}

void param_clear(uint8_t file_id) {

	/* Calculate physical address in FRAM */
	int phybegin = PARAM_MAX_FILESIZE * file_id;
	int crc_table = phybegin + PARAM_MAX_FILESIZE - sizeof(uint16_t);
	int crc_data = crc_table - sizeof(uint16_t);

	/* write table CRC */
	uint16_t crc = 0xFFFF;
	printf("Clear table CRC %u\r\n", crc);
	fm33256b_write_data(crc_table, (void *) &crc, sizeof(uint16_t));

	/* write data CRC */
	crc = 0x55AA;
	printf("Clear data CRC %u\r\n", crc);
	fm33256b_write_data(crc_data, (void *) &crc, sizeof(uint16_t));

}

void param_write_persist(uint16_t addr, uint16_t framaddr, size_t size, void * item, uint8_t flags) {
	fm33256b_write_data(framaddr + addr, item, size);
}

void param_init_persist(param_index_t * mem) {

	if (mem->framaddr == 0)
		return;

	/* Loop all parameters, and reload persistent vars */
	for (unsigned int i = 0; i < mem->count; i++) {
		const param_table_t * param = &mem->table[i];
		if (PARAM_FLAGS & PARAM_F_PERSIST) {
			int count = PARAM_COUNT;
			if (count == 0)
				count = 1;
			for (int j = 0; j < count; j++) {
				uint16_t fromaddr = mem->framaddr + PARAM_ADDR + PARAM_SIZE * j;
				void * toaddr = param_read_addr(PARAM_ADDR + PARAM_SIZE * j, mem, PARAM_SIZE);
				fm33256b_read_data(fromaddr, toaddr, PARAM_SIZE);
			}
		}
	}

}
