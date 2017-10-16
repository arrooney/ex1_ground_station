/*
 * usart.h
 *
 *  Created on: Oct 16, 2017
 *      Author: bbruner
 */

#ifndef SRC_SERIAL_H_
#define SRC_SERIAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/time.h>

typedef void (*serial_callback_t)( uint8_t*, int );

struct serial_dev_t
{
	int fd;
	serial_callback_t serial_callback;
	pthread_t rx_thread;
};

void serial_dev_init( struct serial_dev_t* dev, const char* path, uint32_t baud );
void serial_set_callback( struct serial_dev_t* dev, serial_callback_t callback);
void serial_putstr( struct serial_dev_t* dev, char * buf, int len);
void serial_putc( struct serial_dev_t* dev, char c );
char serial_getc( struct serial_dev_t* dev );
void serial_insert( struct serial_dev_t* dev, char c );
/* return true/false
 */
int serial_messages_waiting( struct serial_dev_t* dev );

#endif /* SRC_SERIAL_H_ */
