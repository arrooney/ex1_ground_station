/*
 * cmd_dfl.c
 *
 *  Created on: Sep 17, 2012
 *      Author: johan
 */

#include <conf_gosh.h>
#include <conf_util.h>

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifndef __linux__
#include <FreeRTOS.h>
#include <task.h>
#include <dev/cpu.h>
#include <dev/usart.h>
#else
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#endif

#ifdef __linux
#include <csp/drivers/usart.h>
#else
#include <dev/usart.h>
#endif
#include <util/console.h>
#include <util/hexdump.h>
#include <util/log.h>
#include <util/clock.h>

#if defined(CONFIG_DRIVER_DEBUG)
#include <util/driver_debug.h>
#endif

#include <command/command.h>

int help_handler(struct command_context * context) {
	command_help(command_args(context));
	return CMD_ERROR_NONE;
}

int sleep_handler(struct command_context * context) {
	unsigned long sleep_ms;

	if (context->argc != 2)
		return CMD_ERROR_SYNTAX;

	sleep_ms = atoi(context->argv[1]);

	if (sleep_ms < 1)
		return CMD_ERROR_SYNTAX;

#ifndef __linux__
	vTaskDelay(sleep_ms * (configTICK_RATE_HZ / 1000.0));
#else
	usleep(sleep_ms*1000);
#endif

	return CMD_ERROR_NONE;
}

int watch_handler(struct command_context * context) {

	int sleep_ms = atoi(context->argv[1]);

	if (sleep_ms < 0)
		return CMD_ERROR_SYNTAX;

	printf("Execution delay: %d\r\n", sleep_ms);

	char * new_command = strstr(command_args(context), " ");

	if (new_command == NULL)
		return CMD_ERROR_SYNTAX;
	else
		new_command = new_command + 1;

	printf("Command: %s\r\n", new_command);

	while(1) {

		if (usart_stdio_msgwaiting())
			break;

		command_run(new_command);

		if (sleep_ms > 0) {
#ifndef __linux__
			vTaskDelay(sleep_ms * (configTICK_RATE_HZ / 1000.0));
#else
			usleep(sleep_ms*1000);
#endif
		}

	}

	return CMD_ERROR_NONE;

}

#define CONTROL(X)  ((X) - '@')

int batch_handler(struct command_context * ctx) {

	char c;
	int quit = 0, execute = 0;
	unsigned int batch_size = 100;
	unsigned int batch_input = 0;
	unsigned int batch_count = 0;
	char * batch[20] = {};
	printf("Type each command followed by enter, hit ctrl+e to end typing, ctrl+x to cancel:\r\n");

	/* Wait for ^q to quit. */
	while (quit == 0) {

		/* Get character */
		c = getchar();

		switch (c) {

		/* CTRL + X */
		case 0x18:
			quit = 1;
			break;

		/* CTRL + E */
		case 0x05:
			execute = 1;
			quit = 1;
			break;

		/* Backspace */
		case CONTROL('H'):
		case 0x7f:
			if (batch_input > 0) {
				putchar('\b');
				putchar(' ');
				putchar('\b');
				batch_input--;
			}
			break;

		case '\r':
			putchar('\r');
			putchar('\n');
			if ((batch[batch_count] != NULL) && (batch_input < batch_size))
				batch[batch_count][batch_input++] = '\r';
			if ((batch[batch_count] != NULL) && (batch_input < batch_size))
				batch[batch_count][batch_input++] = '\0';
			batch_count++;
			batch_input = 0;
			if (batch_count == 20)
				quit = 1;
			break;

		default:
			putchar(c);
			if (batch[batch_count] == NULL) {
				batch[batch_count] = calloc(CONSOLE_BUFSIZ, 1);
			}

			if ((batch[batch_count] != NULL) && (batch_input < batch_size))
				batch[batch_count][batch_input++] = c;
			break;
		}
	}

	if (execute) {
		printf("\r\n");
		for (unsigned int i = 0; i <= batch_count; i++) {
			if (batch[i])
				printf("[%02u] %s\r\n", i, batch[i]);
		}
		printf("Press ctrl+e to execute, or any key to abort\r\n");
		c = getchar();
		if (c != 0x05)
			execute = 0;
	}

	/* Run/Free batch job */
	for (unsigned int i = 0; i <= batch_count; i++) {
		if (execute && batch[i]) {
			printf("EXEC [%02u] %s\r\n", i, batch[i]);
			command_run(batch[i]);
		}
		free(batch[i]);
	}

	return CMD_ERROR_NONE;

}

#ifndef __linux__
int cpu_reset_handler(struct command_context * context) {
	if (cpu_set_reset_cause)
		cpu_set_reset_cause(CPU_RESET_USER);
	cpu_reset();
	return CMD_ERROR_NONE;
}

int ps_handler(struct command_context * context) {

#if (FREERTOS_VERSION >= 8)
	uint32_t task_count = uxTaskGetNumberOfTasks();
	uint32_t total_runtime;

	TaskStatus_t *status = pvPortMalloc(task_count * sizeof(TaskStatus_t));
	if (status == NULL)
		return CMD_ERROR_NOMEM;

	/* Generate raw status information about each task. */
	task_count = uxTaskGetSystemState(status, task_count, &total_runtime);

	if (total_runtime == 0)
		total_runtime = 1;

	for (unsigned int i = 0; i < task_count; i++) {
		double percent = (status[i].ulRunTimeCounter / ((double)total_runtime / 100.0));
		printf("%10s %5u %5lu %5u   %02.05f%%\r\n", status[i].pcTaskName, status[i].eCurrentState, (unsigned long) status[i].uxCurrentPriority, status[i].usStackHighWaterMark, percent);
	}
	vPortFree(status);
#else
	signed char printbuffer[384];
	vTaskList(printbuffer);
	printf("%s", printbuffer);
#endif

	return CMD_ERROR_NONE;
}

int peek_handler(struct command_context * ctx) {

	unsigned int addr, len;

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	if (sscanf(ctx->argv[1], "%x", &addr) != 1)
		return CMD_ERROR_SYNTAX;
	if (sscanf(ctx->argv[2], "%u", &len) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Dumping mem from addr %u len %u\r\n", addr, len);
	hex_dump((void *) addr, len);

	return CMD_ERROR_NONE;
}

int poke_handler(struct command_context * ctx) {

	unsigned int addr, value;

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;
	if (sscanf(ctx->argv[1], "%x", &addr) != 1)
		return CMD_ERROR_SYNTAX;
	if (sscanf(ctx->argv[2], "%x", &value) != 1)
		return CMD_ERROR_SYNTAX;

	printf("Setting addr 0x08%x = 0x08%x\r\n", addr, value);

	/* Dangerous */
	*(unsigned int *) addr = value;

	return CMD_ERROR_NONE;
}

#if configGENERATE_RUN_TIME_STATS
#if FREERTOS_VERSION < 8
int stats_handler(struct command_context *ctx) {
	signed char buffer[512];
	vTaskGetRunTimeStats(buffer);
	printf("%s\r\n", buffer);
	return CMD_ERROR_NONE;
}
#endif
#endif

#else
int exit_handler(struct command_context * context) {
	exit(EXIT_SUCCESS);
	return CMD_ERROR_NONE;
}
#endif

#if defined(CONFIG_DRIVER_DEBUG)
int cmd_driver_debug_toggle(struct command_context * ctx) {
	char * args = command_args(ctx);
	unsigned int driver;
	if (sscanf(args, "%u", &driver) != 1)
		return CMD_ERROR_SYNTAX;

	if (driver > DEBUG_ENUM_MAX)
		return CMD_ERROR_FAIL;
	driver_debug_toggle(driver);
	printf("Debug %u = %u\r\n", driver, driver_debug_enabled(driver));
	return CMD_ERROR_NONE;
}
#endif

int clock_get(struct command_context * ctx) {

	static timestamp_t monotonic = {};

	/* Get monotonic system clock */
	timestamp_t clock;
	clock_get_monotonic(&clock);
	printf("%"PRIu32".%06"PRIu32" ", clock.tv_sec, clock.tv_nsec / 1000);

	/* Check increasing clock */
	if (timestamp_ge(&clock, &monotonic)) {
		printf("ERROR! Clock not monotonic!\r\n");
		while(1);
	}
	timestamp_copy(&clock, &monotonic);

	/* Get UTC time */
	timestamp_t utc_time;
	clock_get_time(&utc_time);
	printf("%"PRIu32".%06"PRIu32" ", utc_time.tv_sec, utc_time.tv_nsec / 1000);
#ifdef HAVE_TIME_H
	printf("%s\r", ctime((time_t *) &utc_time.tv_sec));
#else
	printf("\r\n");
#endif

	return CMD_ERROR_NONE;
}

command_t __root_command cmd_dfl[] = {
	{
		.name = "help",
		.help = "Show help",
		.usage = "<command>",
		.handler = help_handler,
	},{
		.name = "sleep",
		.help = "Sleep X ms",
		.usage = "<time>",
		.handler = sleep_handler,
	},{
		.name = "watch",
		.help = "Run cmd at intervals, abort with key",
		.usage = "<n> <command>",
		.handler = watch_handler,
	},{
#if defined(CONFIG_DRIVER_DEBUG)
		.name = "tdebug",
		.help = "Toggle driver debug",
		.usage = "<level>",
		.handler = cmd_driver_debug_toggle,
	},{
#endif
		.name = "batch",
		.help = "Run multiple commands",
		.handler = batch_handler,
	},
#ifndef __linux__
	{
		.name = "reset",
		.help = "Reset now",
		.handler = cpu_reset_handler,
	},{
		.name = "ps",
		.help = "List tasks",
		.handler = ps_handler,
	},{
		.name = "peek",
		.help = "Dump memory",
		.usage = "<addr> <len>",
		.handler = peek_handler,
	},{
		.name = "poke",
		.help = "Change memory",
		.usage = "<addr> <value>",
		.handler = poke_handler,
	},{
		.name = "clock",
		.help = "Get system clock",
		.handler = clock_get,
	}
#if configGENERATE_RUN_TIME_STATS
#if FREERTOS_VERSION < 8
	{
		.name = "stats",
		.help = "Get runtime stats",
		.handler = stats_handler,
	},
#endif
#endif
#else
	{
		.name = "exit",
		.help = "Exit program",
		.handler = exit_handler,
	},
#endif
};

void cmd_dfl_setup(void) {
	command_register(cmd_dfl);
}
