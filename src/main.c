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
#include <unistd.h>
#include <IOHook.h>
#include <pforth.h>
#endif

#include <sayhi.h>

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
//	console_exit();
}

char * pipe_buffer;
char holdbuff=0;
#ifdef AUTOMATION
#define STARTUP_BUFFER_TIMEOUT 500
static pthread_t forth_thread_handle;

static void* forth_thread( void* arg );
#endif

int main(int argc, char * argv[])
{
	int console_cleanup = 0;
	
#ifdef AUTOMATION
	/* Initialize IOHook.
	 */
	if( IOHook_Init( ) != 0 ) {
		return -1;
	}
#endif
//	print_logo( );
	fflush( stdout );
	usleep( 1000*1000*1 );
	sayhi();
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
	while ((c = getopt(argc, argv, "a:b:c:d:e:hz:")) != -1) {
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
		case 'e':
			console_cleanup = 1;
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
	pthread_setname_np(handle_server, "server");


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
	//console_init();
	console_set_hostname("csp-term");
	static pthread_t handle_console;
	pthread_create(&handle_console, NULL, debug_console, NULL);
	pthread_setname_np(handle_console, "gomshell");
	
#ifdef AUTOMATION
	pthread_create(&forth_thread_handle, NULL, forth_thread, NULL);
	pthread_setname_np(forth_thread_handle, "forth");
#endif

	/* Wait here for console to end */
#ifdef AUTOMATION
	pthread_join(forth_thread_handle, NULL);
#endif
	pthread_join(handle_console, NULL);
	pthread_join(handle_server, NULL);
	return 0;

}

static void gomshell_prime( )
{
	/* Prime the gomshell with a test command.
	 */
	int i;
	const char* test_command = "print hi\n";
	struct CCThreadedQueue* input_buffer;
	input_buffer = IOHook_GetGomshellInputQueue( );
	for( i = 0; i < strlen(test_command); ++i ) {
		CCThreadedQueue_Insert(input_buffer, &test_command[i], COS_BLOCK_FOREVER);
	}	
}

static void gomshell_flush( )
{
	IOHook_Printf_FP printf_fp;
	char msg;
	int i;
	struct CCThreadedQueue* output_buffer;
	CCTQueueError err;
	
	printf_fp = IOHook_GetPrintf( );
	output_buffer = IOHook_GetGomshellOutputQueue( );

	/* Empty the gomshell output buffer.
	 */
	for( ;; ) {
		err = CCThreadedQueue_Remove(output_buffer, &msg, STARTUP_BUFFER_TIMEOUT);
		if( err == CCTQUEUE_OK ) {
			printf_fp("%c", msg);
		}
		else {
			break;
		}
	}

}

#ifdef AUTOMATION
static void* forth_thread( void* arg )
{
 	(void) arg;
	IOHook_Printf_FP printf_fp;
	const char *SourceName = NULL;
	char IfInit = 0;
	char *s;
	char msg;
	int i;
	int Result;
	const char *DicName = PFORTH_DIC_PATH;
	struct CCThreadedQueue* output_buffer;
	CCTQueueError err;
	
	printf_fp = IOHook_GetPrintf( );
	printf_fp("Forth thread running\n");

	/* Flush the gomshell buffer.
	 */
	gomshell_flush( );

	/* Prime the gomshell three times.
	 */
	gomshell_prime( );
	usleep(10*1000);
	gomshell_flush( );
	
	gomshell_prime( );
	usleep(10*1000);
	gomshell_flush( );
	
	gomshell_prime( );
	usleep(10*1000);
	gomshell_flush( );
	
	fflush(stdout);	      	

	/* Disable verbose output at shell prompt.
	 */
	pfSetQuiet(1);

	/* Start the Forth Kernal. This will never return.
	 */
	Result = pfDoForth( DicName, SourceName, IfInit);	

	printf_fp("Unexpected exit from Forth kernal\n");
	pthread_exit(NULL);
	
#ifdef OUTPUT_LOG
	FILE* output_log;
	time_t ltime;

	output_log = fopen(OUTPUT_LOG_NAME, "a+");
	if( output_log == NULL ) {
		pthread_exit(NULL);
	}

	ltime = time(NULL);
	fprintf(output_log, "\n\nNew Entry: %s\n", asctime(localtime(&ltime)));
#endif

	fclose(output_log);
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

