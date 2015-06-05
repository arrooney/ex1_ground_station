/*
 * driver_debug.h
 *
 *  Created on: 03/03/2011
 *      Author: johan
 */

#ifndef DRIVER_DEBUG_H_
#define DRIVER_DEBUG_H_

#include <stdio.h>
#include <conf_util.h>

typedef enum driver_debug_e {
	DEBUG_I2C = 0,
	DEBUG_ISI = 1,
	DEBUG_KISS_HEXDUMP = 2,
	DEBUG_KISS_STDOUT = 3,
	DEBUG_COM_MAC = 4,
	DEBUG_COM_PACKET = 5,
	DEBUG_COM_HEX = 6,
	DEBUG_COM_MORSE = 7,
	DEBUG_FPGA_PARSE = 8,
	DEBUG_FPGA_SSC = 9,
	DEBUG_COM_PREDICT = 10,
	DEBUG_GATOSS_GC = 11,
	DEBUG_OBC_GC = 12,
	DEBUG_OBC_HK_CLIENT_PRINT = 13,
	DEBUG_COM_BER = 14,
	DEBUG_NOVATEL = 15,
	DEBUG_MPU3300 = 16,
	DEBUG_SPEQS = 17,
	DEBUG_FS_LOCK = 18,
	DEBUG_ENUM_MAX = DEBUG_FS_LOCK,
} driver_debug_t;

typedef enum driver_debug_value_e {
	DRIVER_DEBUG_OFF = 0,
	DRIVER_DEBUG_ON = 1,
} driver_debug_value_t;

#if defined(CONFIG_DRIVER_DEBUG)

#define driver_debug(driver, format, ...) { if (driver_debug_enabled(driver)) { printf(format, ##__VA_ARGS__);} };
unsigned int driver_debug_enabled(driver_debug_t driver);
void driver_debug_toggle(driver_debug_t driver);
void driver_debug_set(driver_debug_t driver, driver_debug_value_t);

#else

#define driver_debug(driver, format, ...) {}
#define driver_debug_enabled(...) (0)
#define driver_debug_set(...) {}
#define driver_debug_toggle(...) {}
#endif

#endif /* DRIVER_DEBUG_H_ */
