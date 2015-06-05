/**
 * Mission Control Server
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#include <param/param.h>
#include <ftp/ftp_server.h>
#include <csp/csp.h>
#include <util/log.h>
#include <csp-term.h>

void * task_server(void * parameters) {

	/* Create socket */
	csp_socket_t * sock = csp_socket(0);

	/* Bind every port to socket*/
	csp_bind(sock, CSP_ANY);

	/* Create 10 connections backlog queue */
	csp_listen(sock, 10);

	/* Pointer to current connection and packet */
	csp_conn_t * conn;
	csp_packet_t * packet;

	/* Setup FTP server RAM */
	ftp_register_backend(BACKEND_RAM, &backend_ram);
	ftp_register_backend(BACKEND_NEWLIB, &backend_newlib);

	/* Process incoming connections */
	while (1) {

		/* Wait for connection, 1000 ms timeout */
		if ((conn = csp_accept(sock, 1000)) == NULL)
			continue;

		/* Spawn new task for FTP */
		if (csp_conn_dport(conn) == CSPTERM_PORT_FTP) {
			pthread_t handle_ftp;
			pthread_attr_t attr;

			/* Create thread as detached to prevent memory leak */ 
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			extern void * task_ftp(void * conn_param);
			pthread_create(&handle_ftp, &attr, task_ftp, conn);
			continue;
		}

		/* Read packets. Timout is 1000 ms */
		while ((packet = csp_read(conn, 1000)) != NULL) {

			switch(csp_conn_dport(conn)) {

				case CSPTERM_PORT_RPARAM:
					rparam_service_handler(conn, packet);
				break;

				case CSPTERM_PORT_PRINT:
					printf("%*s\r\n",packet->length,packet->data);
					//csp_buffer_free(packet);
					csp_close(conn);
				default: {
					csp_service_handler(conn, packet);
					csp_close(conn);
					break;
				}

			}

		}

		/* Close current connection, and handle next */
		csp_close(conn);

	}

	return NULL;

}

