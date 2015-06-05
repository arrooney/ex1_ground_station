/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_TEST_H_
#define PARAM_TEST_H_

#include <stddef.h>
#include <stdint.h>
#include <param/param_types.h>

/**
 * Define required memory space,
 * This is used by the HEAP/BSS based memory mappers
 */
#define PARAM_TEST_SIZE			0xFF

/**
 * Define memory space
 */
#define PARAM_TEST_UINT8(i)		(0x10 + (sizeof(uint8_t) * i))
#define PARAM_TEST_UINT16		0x20
#define PARAM_TEST_UINT32		0x30
#define PARAM_TEST_UINT64		0x40
#define PARAM_TEST_INT8			0x50
#define PARAM_TEST_INT16		0x60
#define PARAM_TEST_INT32		0x70
#define PARAM_TEST_INT64		0x80
#define PARAM_TEST_FLOAT		0x90
#define PARAM_TEST_DOUBLE(i)	(0xA0 + (sizeof(double) * i))
#define PARAM_TEST_STRING		0xB0
#define PARAM_TEST_STRING_SIZE	0x10
#define PARAM_TEST_DATA			0xC0
#define PARAM_TEST_DATA_SIZE	0x10

/**
 * Setup info about parameters
 */
static const param_table_t param_test[] = {
		{.name = "uint8", 	.addr = PARAM_TEST_UINT8(0),	.type = PARAM_UINT8,	.size = sizeof(uint8_t),	.count = 3},
		{.name = "uint16", 	.addr = PARAM_TEST_UINT16, 		.type = PARAM_UINT16, 	.size = sizeof(uint16_t)},
		{.name = "uint32", 	.addr = PARAM_TEST_UINT32, 		.type = PARAM_UINT32, 	.size = sizeof(uint32_t)},
		{.name = "uint64", 	.addr = PARAM_TEST_UINT64, 		.type = PARAM_UINT64, 	.size = sizeof(uint64_t)},
		{.name = "int8", 	.addr = PARAM_TEST_INT8, 		.type = PARAM_INT8, 	.size = sizeof(int8_t)},
		{.name = "int16", 	.addr = PARAM_TEST_INT16, 		.type = PARAM_INT16, 	.size = sizeof(int16_t)},
		{.name = "int32", 	.addr = PARAM_TEST_INT32, 		.type = PARAM_INT32, 	.size = sizeof(int32_t)},
		{.name = "int64", 	.addr = PARAM_TEST_INT64, 		.type = PARAM_INT64, 	.size = sizeof(int64_t)},
		{.name = "float", 	.addr = PARAM_TEST_FLOAT, 		.type = PARAM_FLOAT, 	.size = sizeof(float), .count = 2},
		{.name = "double", 	.addr = PARAM_TEST_DOUBLE(0), 	.type = PARAM_DOUBLE, 	.size = sizeof(double), .count = 2},
		{.name = "string", 	.addr = PARAM_TEST_STRING, 		.type = PARAM_STRING, 	.size = 2, .count = 3},
		{.name = "data", 	.addr = PARAM_TEST_DATA, 		.type = PARAM_DATA, 	.size = PARAM_TEST_DATA_SIZE, .count = 2},
};

#define PARAM_TEST_COUNT (sizeof(param_test) / sizeof(param_test[0]))

#endif /* PARAM_TEST_H_ */
