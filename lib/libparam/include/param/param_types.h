/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_TYPES_H_
#define PARAM_TYPES_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#define PARAM_COUNT	pgm_read_byte(&param->count)
#define PARAM_SIZE	pgm_read_byte(&param->size)
#define PARAM_TYPE	pgm_read_byte(&param->type)
#define PARAM_ADDR	pgm_read_word(&param->addr)
#define PARAM_FLAGS pgm_read_byte(&param->flags)
#define PARAM_PRINT_NAME()	printf_P(param->name)
#define param_memcpy	memcpy_P
#define param_strncmp	strncmp_P
#else
#define PARAM_COUNT param->count
#define PARAM_SIZE	param->size
#define PARAM_TYPE	param->type
#define PARAM_ADDR  param->addr
#define PARAM_FLAGS param->flags
#define PARAM_PRINT_NAME()	printf("%s", param->name)
#define param_memcpy	memcpy
#define param_strncmp	strncmp
#endif

#define MAX_PARAM_NAME_LEN		14
#define RPARAM_QUERY_MAX_LEN	180
#define PARAM_MAX_FILESIZE		0x400

enum param_flags {
	PARAM_F_NOCALLBACK = (1 << 2),	//! Avoid using callback
	PARAM_F_READONLY = (1 << 1),	//! This parameter should not be written to by remote systems
	PARAM_F_PERSIST = (1 << 0),		//! These parameters should be reloaded from non-volatile memory at boot time
};

/**
 * List of supported parameter types.
 */
typedef enum __attribute__((__packed__)) {
	PARAM_UINT8,
	PARAM_UINT16,
	PARAM_UINT32,
	PARAM_UINT64,
	PARAM_INT8,
	PARAM_INT16,
	PARAM_INT32,
	PARAM_INT64,
	PARAM_X8,
	PARAM_X16,
	PARAM_X32,
	PARAM_X64,
	PARAM_DOUBLE,
	PARAM_FLOAT,
	PARAM_STRING,
	PARAM_DATA,
} param_type_t;

typedef enum __attribute__((__packed__)) {
	PARAM_CONF_FALLBACK = 0,
	PARAM_CONF_DEFAULT = 1,
	PARAM_CONF_STORED = 2,
} param_config_id;

typedef struct param_pool_s {
	struct param_pool_s * prev;
	struct param_pool_s * next;
	uint32_t timestamp;
	uint8_t packed[0];
} param_pool_t;

/**
 * Table format for public type declarations
 */
typedef struct param_table_s {
	uint16_t addr;
	param_type_t type;
	uint8_t size;
	uint8_t count;
	uint8_t flags;
	char name[MAX_PARAM_NAME_LEN];
} param_table_t;

struct param_index_s;
typedef void (*param_callback_func)(uint16_t addr, struct param_index_s * index);

/**
 * Parameter table index
 */
typedef struct param_index_s {
	uint8_t mem_id;
	const param_table_t * table;
	unsigned int count;
	void * (*read)(uint16_t addr, size_t size);
	void (*write)(uint16_t addr, size_t size, void * item, uint8_t flags);
	void * physaddr;
	uint32_t size;
	uint16_t framaddr;
	uint16_t table_chksum;
	void * lock;
	int lock_inited;

	//! Set this to the actual size of the memory allocation if this is a scratch memory
	unsigned int scratch_size;

	//! This pointer is used by hk_store
	param_pool_t * pool;
	unsigned int pool_packsize;

	param_callback_func callback;
} param_index_t;

typedef uint8_t param_mem;

typedef enum __attribute__ ((packed)) rparam_action_e {
	RPARAM_GET = 0x00,
	RPARAM_REPLY = 0x55,
	RPARAM_SET = 0xFF,
	RPARAM_SET_TO_FILE = 0xEE,
	RPARAM_TABLE = 0x44,
	RPARAM_COPY = 0x77,
	RPARAM_LOAD = 0x88,
	RPARAM_SAVE = 0x99,
	RPARAM_CLEAR = 0xAA,
} rparam_action;

typedef enum __attribute__ ((packed)) rparam_reply_e {
	RPARAM_SET_OK = 1,
	RPARAM_LOAD_OK = 2,
	RPARAM_SAVE_OK = 3,
	RPARAM_COPY_OK = 4,
	RPARAM_CLEAR_OK = 5,
	RPARAM_ERROR = 0xFF,
} rparam_reply;

typedef struct __attribute__ ((packed)) rparam_frame_s {
	rparam_action action;									//! Type is the action performed
	uint8_t mem;											//! Memory area to work on (0 = RUNNING, 1 = SCRATCH)
	uint16_t length;										//! Length of the payload in bytes
	uint16_t checksum;										//! Fletcher's checksum
	uint16_t seq;
	uint16_t total;
	union {
		uint16_t addr[0];									//! action = PARAM_GET
		uint8_t packed[0];									//! action = PARAM_REPLY | PARAM_SET
		struct {
			uint8_t from;
			uint8_t to;
		} copy;
		struct {
			uint8_t id;
		} clear;
	};
} rparam_query;

#endif /* PARAM_TYPES_H_ */
