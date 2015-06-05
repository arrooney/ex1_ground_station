/*
 * driver_debug.c
 *
 *  Created on: 03/03/2011
 *      Author: johan
 */

#include <conf_util.h>

#if defined(CONFIG_DRIVER_DEBUG)
#include <util/log.h>
#include <util/driver_debug.h>

static unsigned char driver_debug_switch[DEBUG_ENUM_MAX+1] = {0};

inline void driver_debug_toggle(driver_debug_t driver) {
	driver_debug_switch[driver] = (driver_debug_switch[driver] + 1) % 2;
}

inline void driver_debug_set(driver_debug_t driver, driver_debug_value_t value) {
	driver_debug_switch[driver] = value % 2;
}

inline unsigned int driver_debug_enabled(driver_debug_t driver) {
	return driver_debug_switch[driver];
}

#endif
