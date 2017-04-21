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
/* IO Controller */
#ifdef AUTOMATION
#include <CCSoftSerialDev.h>
#include <unistd.h>
#include <CCSoftSerialBus.h>
#include <IOHook.h>

#define COS_PRINT_ACK_TIMEOUT 15
#define IO_BUS_CHANNEL_LENGTH 5096
#define IO_MASTER_PRIO 2
#define IO_FORTH_PRIO 1

static struct CCSoftSerialDev io_slave;
struct CCSoftSerialDev io_master;
struct CCSoftSerialBus io_bus;
#endif

const vmem_t vmem_map[] = {{0}};

static void print_help(void) {
	printf(" usage: csp-term <-d|-c|-z> [optargs]\r\n");
	printf("  -d DEVICE,\tSet device (default: /dev/ttyUSB0)\r\n");
	printf("  -c DEVICE,\tSet can device (default: can0)\r\n");
	printf("  -z SERVER,\tSet ZMQ server (default: localhost)\r\n");
	printf("  -a ADDRESS,\tSet address (default: 8)\r\n");
	printf("  -b BAUD,\tSet baud rate (default: 500000)\r\n");
	printf("  -h,\t\tPrint help and exit\r\n");
}
static void print_logo( );
static void exithandler(void) {
	console_exit();
}

char * pipe_buffer;
char holdbuff=0;
#ifdef AUTOMATION
static pthread_t terminal_input_thread_handle;

static void* terminal_input_thread( void* arg );
#endif

int main(int argc, char * argv[])
{
#ifdef AUTOMATION
	/* Initialize IOHook.
	 */
	if( CCSoftSerialBus(&io_bus, sizeof(char), IO_BUS_CHANNEL_LENGTH) != COBJ_OK ) {
		return -1;
	}
	if( CCSoftSerialDevSlave(&io_slave, IO_SLAVE_ID, &io_bus) != COBJ_OK ) {
		return -1;
	}
	if( CCSoftSerialDevMaster(&io_master, IO_MASTER_PRIO, IO_MASTER_ID, &io_bus) != COBJ_OK ) {
		return -1;
	}
	if( IOHook_Init(&io_slave) != CTRUE ) {
		return -1;
	}

	pthread_create(&terminal_input_thread_handle, NULL, terminal_input_thread, NULL);
#endif
	print_logo( );
	fflush( stdout );
	usleep( 1000*1000*1 );

	atexit(exithandler);

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
	while ((c = getopt(argc, argv, "a:b:c:d:hz:")) != -1) {
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
		default:
			exit(EXIT_FAILURE);
		}
	}

	/**
	 * CSP / Network
	 */
	csp_set_hostname("csp-term");
	csp_set_model("CSP Term");
	csp_buffer_init(400, 512);
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

	/**
	 * ZMQ interface
	 */
	if (use_zmq == 1) {
		csp_zmqhub_init(addr, zmqhost);
		csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_zmqhub, CSP_NODE_MAC);
	}

	/**
	 * CAN Interface
	 */
	if (use_can == 1) {
		struct csp_can_config conf = {.ifc = ifc};
		csp_can_init(CSP_CAN_MASKED, &conf);
		csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_can, CSP_NODE_MAC);
	}

	/* LibADCS is weak linked */
#ifdef ENABLE_ADCS_CLIENT
	adcs_node_set(1);
#endif

	/**
	 * liblog setup
	 */
	#define LOG_STORE_SIZE 0x200
	static uint8_t buf[LOG_STORE_SIZE] = {};
	log_store_init((vmemptr_t) buf, LOG_STORE_SIZE);

	/**
	 * Parameter system
	 */
	param_index_set(0, (param_index_t) {.table = param_test, .count = PARAM_TEST_COUNT, .physaddr = malloc(PARAM_TEST_SIZE), .size = PARAM_TEST_SIZE});

	/**
	 * Tasks
	 */

	/* Router */
	csp_route_start_task(1000, 0);

	/* Server */
	void * task_server(void * parameters);
	static pthread_t handle_server;
	pthread_create(&handle_server, NULL, task_server, NULL);


	void cmd_eps_setup(void);
	cmd_eps_setup();
/*
    void cmd_tele_setup(void);
	cmd_tele_setup();
*/
	void cmd_utils_setup(void);
	cmd_utils_setup();


	// TODO finish fixing hub gosh commands.

	/* Console */
	command_init();
	console_init();
	console_set_hostname("csp-term");
	static pthread_t handle_console;
	pthread_create(&handle_console, NULL, debug_console, NULL);

	/* Wait here for console to end */
	pthread_join(handle_console, NULL);
	pthread_join(handle_server, NULL);
#ifdef AUTOMATION
	pthread_join(terminal_input_thread_handle, NULL);
#endif

	return 0;

}

#ifdef AUTOMATION
static void* terminal_input_thread( void* arg )
{
	(void) arg;
	char msg;
	IOHook_Getchar_FP getchar_fp;
	IOHook_Printf_FP printf_fp;
	CCSoftSerialError err;

	printf_fp = IOHook_GetPrintf( );
	getchar_fp = IOHook_GetGetchar( );
	printf_fp("Gomshell IO Controller Running\n")
	CCSoftSerialDev_Select(&io_master, IO_SLAVE_ID, COS_BLOCK_FOREVER);
	for( ;; ) {
		for( ;; ) {
			err = CCSoftSerialDev_Read(&io_master, &msg, COS_PRINT_ACK_TIMEOUT);
			if( err == CCSOFTSERIAL_OK ) {
				printf_fp("%c", msg);
			}
			else {
				break;
			}
		}
		msg = getchar_fp( );
		CCSoftSerialDev_Write(&io_master, &msg, COS_BLOCK_FOREVER);
	}
	pthread_exit(NULL);
}
#endif

static void print_logo( )
{
	printf( "               ,        ,			\n" );
	printf( "              /(        )`			\n" );
	printf( "              \\ \\___   / |		\n" );
	printf( "              /- _  `-/  '			\n" );
	printf( "             (/\\/ \\ \\   /\\		\n" );
	printf( "             / \\/   | `    \\		\n" );
	printf( "            O O   ) /    |			\n" );
	printf( "            `-^--'`<     '			\n" );
	printf( "           (_.)  _  )   /			\n" );
	printf( "            `.___/`    /			\n" );
	printf( "              `-----' /			\n" );
	printf( "<----.     __ / __   \\			\n" );
	printf( "<----|====O)))==) \\) /====		\n" );
	printf( "<----'    `--' `.__,' \\			\n" );
    printf( "             |        |			\n" );
	printf( "              \\       /			\n" );
	printf( "        ______( (_  / \\______		\n" );
	printf( "       ,'  ,-----'   |        \\	\n" );
	printf( "       `--{__________)        \\/ 	\n" );
}

