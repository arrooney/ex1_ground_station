/**
 * CSP-Terminal
 *
 * @author Johan De Claville Christiansen
 * Copyright 2010-2015 GomSpace ApS. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include "nanopower2.h"
#include <conf_cspterm.h>
#include <csp-term.h>
#ifdef WITH_ADCS
#include <conf_adcs.h>
#endif
/* Parameter system */
#include <param/param.h>
#include <param/param_example_table.h>
/* Log system */
#include <log/log.h>
/* CSP */
#include <csp/csp.h>
#include <csp/interfaces/csp_if_kiss.h>
#include <csp/interfaces/csp_if_can.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#include <csp/drivers/usart.h>
/* Drivers / Util */
#include <util/console.h>
#include <util/log.h>
#include <util/vmem.h>
#ifdef AUTOMATION
#include <unistd.h>
#include <pforth.h>
#include <CCThreadedQueue.h>
#include <CCArrayQueue.h>
#endif

const vmem_t vmem_map[] = {{0}};
char * pipe_buffer;
char holdbuff=0;
#ifdef AUTOMATION
struct CCThreadedQueue gomshell_input;
static struct CCArrayQueue gomshell_input_backbone;
static pthread_t forth_thread_handle;
static void* forth_thread( void* arg );
static char *DicName = "pforth/fth/pforth.dic";
static char DicName_[100];
static char *SrcName = NULL;
static char SrcName_[100];
static char IfInit = 0;
#endif

static void print_help(void) {
	printf(" usage: csp-term <-d|-c|-z> [optargs]\r\n");
	printf("  -d DEVICE,\tSet device (default: /dev/ttyUSB0)\r\n");
	printf("  -c DEVICE,\tSet can device (default: can0)\r\n");
	printf("  -z SERVER,\tSet ZMQ server (default: localhost)\r\n");
	printf("  -a ADDRESS,\tSet address (default: 8)\r\n");
	printf("  -b BAUD,\tSet baud rate (default: 500000)\r\n");
	printf("  -h,\t\tPrint help and exit\r\n");
}


int main(int argc, char * argv[])
{
#ifdef AUTOMATION
	/* Initialize queue for forth -> gomshell IO 
	 */
	CError err;
	err = CCArrayQueue(&gomshell_input_backbone, sizeof(char), 512);
	if( err != COBJ_OK ) {
		printf("Error creating forth->gomshell queue\n");
		return -1;
	}
	err = CCThreadedQueue(&gomshell_input, &gomshell_input_backbone.ciqueue);
	if( err != COBJ_OK ) {
		printf("Error creating forth->gomshell threaded queue\n");
		return -1;
	}	
#endif
	/* Config */
	uint8_t addr = 8;

	/* KISS STUFF */
	char * device = "/dev/ttyUSB0";
	uint32_t baud = 500000;
	uint8_t use_kiss = 0;

	/* CAN STUFF */
	char * ifc = "can0";
	uint8_t use_can = 0;

	/* ZMQ STUFF */
	char zmqhost[100] = "localhost";
	uint8_t use_zmq = 0;

	pipe_buffer=calloc(1,sizeof(char));

	/**
	 * Parser
	 **/
	int c;
	while ((c = getopt(argc, argv, "l:is:a:b:c:d:hz:")) != -1) {
		switch (c) {
		case 'a':
			addr = atoi(optarg);
			break;
		case 'b':
			baud = atoi(optarg);
			break;
		case 'c':
			use_can = 1;
			ifc = optarg;
			break;
		case 'd':
			device = optarg;
			use_kiss = 1;
			break;
		case 'h':
			print_help();
			exit(0);
		case 'z':
			strcpy(zmqhost, optarg);
			use_zmq = 1;
			break;
		case '?':
			return 1;

		/* 'l', 's', and 'i' are pforth options. */
		case 'l':
			strcpy(DicName_, optarg);
			DicName = DicName_;
			printf("Using Forth dictionary %s\n", DicName);
			break;
		case 'i':
			printf("Setting IfInit to TRUE\n");
			IfInit = 1;
			DicName = NULL;
			break;
		case 's':
			strcpy(SrcName_, optarg);
			SrcName = SrcName_;
			printf("Running Forth source %s\n", SrcName);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	 /* CSP / Network */
	csp_set_hostname("csp-term");
	csp_set_model("CSP Term");
	//csp_buffer_init(400, 512);
	csp_buffer_init(2048, 512);
	csp_init(addr);
	csp_rdp_set_opt(6, 30000, 16000, 1, 8000, 3);

	/**
	 * KISS interface
	 */
	if (use_kiss == 1) {
		static csp_iface_t csp_if_kiss;
		static csp_kiss_handle_t csp_kiss_driver;
		static const char * kiss_name = "KISS";


		csp_kiss_init(&csp_if_kiss, &csp_kiss_driver, usart_putc, usart_insert, kiss_name);
		struct usart_conf conf = {.device = device, .baudrate = baud};
		usart_init(&conf);
		void my_usart_rx(uint8_t * buf, int len, void * pxTaskWoken) {
			csp_kiss_rx(&csp_if_kiss, buf, len, pxTaskWoken);
		}
		usart_set_callback(my_usart_rx);

		csp_rtable_set(0, 0, &csp_if_kiss, CSP_NODE_MAC);
/*
		csp_route_set(1, &csp_if_kiss, CSP_NODE_MAC);
		csp_route_set(2, &csp_if_kiss, CSP_NODE_MAC);
		csp_route_set(3, &csp_if_kiss, CSP_NODE_MAC);
		csp_route_set(5, &csp_if_kiss, CSP_NODE_MAC);
*/

	}

	/* ZMQ interface */
	if (use_zmq == 1) {
		csp_zmqhub_init(addr, zmqhost);
		csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_zmqhub, CSP_NODE_MAC);
	}

	 /* CAN Interface */
	if (use_can == 1) {
		struct csp_can_config conf = {.ifc = ifc};
		csp_can_init(CSP_CAN_MASKED, &conf);
		csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_can, CSP_NODE_MAC);
	}

	/* LibADCS is weak linked */
#ifdef ENABLE_ADCS_CLIENT
	adcs_node_set(1);
#endif

	 /* liblog setup */
	#define LOG_STORE_SIZE 0x200
	static uint8_t buf[LOG_STORE_SIZE] = {};
	log_store_init((vmemptr_t) buf, LOG_STORE_SIZE);

	/* Parameter system */
	param_index_set(0, (param_index_t) {.table = param_test, .count = PARAM_TEST_COUNT, .physaddr = malloc(PARAM_TEST_SIZE), .size = PARAM_TEST_SIZE});

	/* Router */
	csp_route_start_task(1000, 0);

	/* Server */
	void * task_server(void * parameters);
	static pthread_t handle_server;
	pthread_create(&handle_server, NULL, task_server, NULL);
	pthread_setname_np(handle_server, "server");


	void cmd_eps_setup(void);
	cmd_eps_setup();

	void cmd_utils_setup(void);
	cmd_utils_setup();

	/* Console */
	command_init();
	console_set_hostname("Current time: ");
	static pthread_t handle_console;
	pthread_create(&handle_console, NULL, debug_console, NULL);
	pthread_setname_np(handle_console, "gomshell");
	
#ifdef AUTOMATION
	sleep(1);
	printf("Initializing Forth kernal thread\n");
	pthread_create(&forth_thread_handle, NULL, forth_thread, NULL);
	pthread_setname_np(forth_thread_handle, "forth");
#endif

	/* Wait here for EVER muahahaha
	 */
#ifdef AUTOMATION
	pthread_join(forth_thread_handle, NULL);
#endif
	pthread_join(handle_console, NULL);
	pthread_join(handle_server, NULL);
	return 0;

}


#ifdef AUTOMATION
static void* forth_thread( void* arg )
{
 	(void) arg;
	char *s;
	char msg;
	int i;
	int Result;

	struct CCThreadedQueue* output_buffer;
	CCTQueueError err;

	printf("Welcome to Ex-Alta 1's interactive Forth shell.\n");

	/* Disable verbose output of forth interpreter. Set to 0 if you
	 * like verbose things.
	 */
	pfSetQuiet(1);

	/* Start the Forth Kernal. This will never return.
	 */
	Result = pfDoForth( DicName, SrcName, IfInit);	

	printf("Exit from Forth kernal, condition %d\n", Result);
	if( Result == 0 ) {
		exit(0);
	} else {
		exit(-1);
	}
	return NULL;
}
#endif
