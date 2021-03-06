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
#include <time.h>

#include <param/param.h>
#include <ftp/ftp_server.h>
#include <csp/csp.h>
#include <util/log.h>
#include <csp-term.h>
#include <CCThreadedQueue.h>
#include <CCArrayQueue.h>

#include <nanopower2.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define LGREEN   "\x1b[32;1m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"
#define GREY	   "\x1b[30;1m"

#define WOD_BEACON_PORT 32
#define CALLSIGN_PORT 33
#define CMD_RSP_PORT 30

eps_hk_t beacon_data;
FILE* beacon_dump;

void * task_server(void * parameters) {

	uint8_t * mssg;
	uint8_t callsign[6] = {};
	uint16_t vbatt;

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

		/* Read packets. Timout is 5000 ms */
		while ((packet = csp_read(conn, 5000)) != NULL) {

			switch(csp_conn_dport(conn)) {

				case CSPTERM_PORT_RPARAM:
					rparam_service_handler(conn, packet);
				break;

				case CSPTERM_PORT_PRINT:
					mssg=(uint8_t *) malloc((packet->length)+1);
					strncpy(mssg,packet->data,packet->length);
					mssg[packet->length]=0;

					switch(packet->id.src){
					case 1:
						printf(LGREEN "%d Nanomind " C_RESET GREY "#" C_RESET GREEN" %s\n\r" C_RESET,time(NULL),mssg);
						break;
					case 2:
						printf("%d Nanohub # %s\n",time(NULL),mssg);
						break;
					case 3:
						printf("%d Nanopower # %s\n",time(NULL),mssg);
						break;
					case 5:
						printf("%d Nanocomm # %s\n",time(NULL),mssg);
						break;
					default:
						printf("%d Node:%d # %s\n",time(NULL),packet->id.src,mssg);
						}
					free(mssg);
					fflush(stdout);
					csp_close(conn);
					break;
				case WOD_BEACON_PORT:
					printf("%d Got beacon. \n", time(NULL));
					vbatt = (packet->data[7] << 8) + packet->data[6];
                    			for(int i=0;i<6;i++)
                    			{
                        			callsign[i] = packet->data[131+3+i];
                    			}
	                		printf("Callsign: %s \n", callsign);
					//printf_fp("VBatt: %d \n", vbatt);
					memcpy(&beacon_data, packet->data, 131);
					//eps_hk_print(&beacon_data);
					//fflush(stdout);
					//printf_fp("Beacon puke\n");
					beacon_dump = fopen("beacon.csv","a");			
					for(int i=0;i<packet->length;i++)
					{
						fprintf(beacon_dump, "%X", packet->data[i]);
						fprintf(beacon_dump, ",");
					}
					fprintf(beacon_dump, "\n");
					fflush(beacon_dump);
					fclose(beacon_dump);
					csp_close(conn);
					break;
				case CALLSIGN_PORT:
					printf("Got callsign. \n");
					printf("%s \n", packet->data);
					csp_close(conn);
					break;
				case CMD_RSP_PORT: {
					unsigned char* resp_bin = packet->data;
					uint32_t rsp = (packet->data[3] << 24)
						+ (packet->data[2] << 16)
						+ (packet->data[1] << 8)
						+ (packet->data[0]);
					printf("cmd resp: %d\n", (int) rsp);
					fflush(stdout);
					csp_close(conn);
					break;
				}
				default: {
					csp_service_handler(conn, packet);
					csp_close(conn);
					break;
				}

			}
			csp_buffer_free(packet);

		}

		/* Close current connection, and handle next */
		csp_close(conn);

	}

	return NULL;

}

