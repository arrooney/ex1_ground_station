/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "serial.h"



static void *serial_dev_rx_thread(void *vptr_args);

int serial_dev_init( struct serial_dev_t* dev, const char* path, uint32_t baud )
{

	struct termios options;
	int brate = 0;

	dev->serial_callback = NULL;
	dev->fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (dev->fd < 0) {
		printf("Failed to open %s: %s\r\n", path, strerror(errno));
		return -1;
	}

	switch(baud) {
    case 4800:    brate=B4800;    break;
    case 9600:    brate=B9600;    break;
    case 19200:   brate=B19200;   break;
    case 38400:   brate=B38400;   break;
    case 57600:   brate=B57600;   break;
    case 115200:  brate=B115200;  break;
#ifndef CSP_MACOSX
    case 460800:  brate=B460800;  break;
    case 500000:  brate=B500000;  break;
    case 921600:  brate=B921600;  break;
    case 1000000: brate=B1000000; break;
    case 1500000: brate=B1500000; break;
    case 2000000: brate=B2000000; break;
    case 2500000: brate=B2500000; break;
    case 3000000: brate=B3000000; break;
#endif
    }

	tcgetattr(dev->fd, &options);
	cfsetispeed(&options, brate);
	cfsetospeed(&options, brate);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	options.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 1;
	tcsetattr(dev->fd, TCSANOW, &options);
	if (tcgetattr(dev->fd, &options) == -1)
		perror("error setting options");
	fcntl(dev->fd, F_SETFL, 0);

	/* Flush old transmissions */
	if (tcflush(dev->fd, TCIOFLUSH) == -1)
		printf("Error flushing serial port - %s(%d).\n", strerror(errno), errno);

	if (pthread_create(&dev->rx_thread, NULL, serial_dev_rx_thread, dev) != 0)
		return -1;
	return 0;

}

void serial_set_callback( struct serial_dev_t* dev, serial_callback_t callback)
{
	dev->serial_callback = callback;
}

void serial_putstr( struct serial_dev_t* dev, char * buf, int len)
{
	write(dev->fd, buf, len);
}

void serial_putc( struct serial_dev_t* dev, char c )
{
	write(dev->fd, &c, 1);
}

char serial_getc( struct serial_dev_t* dev )
{
	char c;
	if (read(dev->fd, &c, 1) != 1) return 0;
	return c;
}

void serial_insert( struct serial_dev_t* dev, char c )
{
	serial_putc(dev, c);
}

/* return true/false
 */
int serial_messages_waiting( struct serial_dev_t* dev )
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(dev->fd, &fds);
  select(dev->fd+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(dev->fd, &fds));
}

static void *serial_dev_rx_thread(void *vptr_args)
{
	struct serial_dev_t* dev = vptr_args;
	unsigned int length;
	uint8_t * cbuf = malloc(100000);

	while (1) {
		length = read(dev->fd, cbuf, 300);
		if (length <= 0) {
			perror("Error: ");
			exit(1);
		}
		if (dev->serial_callback)
			dev->serial_callback(cbuf, length);
	}
	return NULL;
}
