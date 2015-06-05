#include <param/param_types.h>
#include "FreeRTOS.h"
#include "semphr.h"

void param_lock(param_index_t * mem) {
	if (mem->lock_inited == 0)
		return;
	xSemaphoreTake(mem->lock, portMAX_DELAY);
}

void param_unlock(param_index_t * mem) {
	if (mem->lock_inited == 0)
		return;
	xSemaphoreGive(mem->lock);
}

void param_lock_init(param_index_t * mem) {
	mem->lock = xSemaphoreCreateMutex();
	if (mem->lock != NULL)
		mem->lock_inited = 1;
	else
		mem->lock_inited = 0;
	return;
}
