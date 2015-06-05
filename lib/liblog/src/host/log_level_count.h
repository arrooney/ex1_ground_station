/*
 * log_level_count.h
 *
 *  Created on: 24/02/2015
 *      Author: johan
 */

#ifndef LOG_LEVEL_COUNT_H_
#define LOG_LEVEL_COUNT_H_

#include <log/log_types.h>

void log_level_count(log_level_t level);
void log_level_count_init(void);
void log_level_count_reset(void);

#endif /* LOG_LEVEL_COUNT_H_ */
