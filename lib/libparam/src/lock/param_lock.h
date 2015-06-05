/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_LOCK_H_
#define PARAM_LOCK_H_

#include <param/param_types.h>

/**
 * Lock functions
 */
void param_lock(param_index_t * mem);
void param_unlock(param_index_t * mem);
void param_lock_init(param_index_t * mem);

#endif /* PARAM_LOCK_H_ */
