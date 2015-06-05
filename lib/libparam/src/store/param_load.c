#include <param/param_types.h>
#include <param/param_fletcher.h>
#include <param_host.h>

param_config_id param_load_fallback(param_index_t * mem, uint8_t fno, uint8_t fno_dfl, void (*fallback)(void), char * name) {
	if (param_load(fno, mem) < 0) {
		if (param_load(fno_dfl, mem) < 0) {
			fallback();
			return PARAM_CONF_FALLBACK;
		} else {
			return PARAM_CONF_DEFAULT;
		}
	} else {
		return PARAM_CONF_STORED;
	}
}
