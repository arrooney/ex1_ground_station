/**
 * @file command.c
 * Command storage
 *
 * @author Jeppe Ledet-Pedersen
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

/** TODO: Partial extension */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef __linux__
#include <FreeRTOS.h>
#include <semphr.h>
SemaphoreHandle_t command_mutex = NULL;
#endif

#include <command/command.h>
#include <conf_gosh.h>
#include <util/console.h>
#include <util/log.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#define cmd_strcmp strcmp_P
#define cmd_strncmp strncmp_P
#define cmd_strlen strlen_P
#define cmd_strcpy strcpy_P
#define cmd_read_ptr(ptr) ((void *) pgm_read_word(ptr))
#define cmd_read_int(ptr) pgm_read_word(ptr)
#else
#define cmd_strcmp strcmp
#define cmd_strncmp strncmp
#define cmd_strlen strlen
#define cmd_strcpy strcpy
#define cmd_read_ptr(ptr) *ptr
#define cmd_read_int(ptr) *ptr
#endif

extern volatile unsigned int __command_start __attribute__ ((__weak__));
extern volatile unsigned int __command_end __attribute__ ((__weak__));

static int command_count = 0;
static command_t *commands = NULL;

void command_lock(void) {
#ifndef __linux__
	xSemaphoreTake(command_mutex, configTICK_RATE_HZ);
#endif
}

void command_unlock(void) {
#ifndef __linux__
	xSemaphoreGive(command_mutex);
#endif
}

/**
 * Remove leading and trailing whitespace from string in-place
 * @param String to strip
 */
void strstrip(char *s) {
	char *start;
	char *end;

	/* Exit if param is NULL pointer */
	if (s == NULL)
		return;

	/* Skip over leading whitespace */
	start = s;
	while ((*start) && isspace((unsigned char) *start))
		start++;

	/* Is string just whitespace? */
	if (!(*start)) {
		*s = 0x00; //! Truncate entire string
		return;
	}

	/* Find end of string */
	end = start;
	while (*end)
		end++;

	/* Step back from NUL */
	end--;

	/* Step backward until first non-whitespace */
	while ((end != start) && isspace((unsigned char) *end))
		end--;

	/* Chop off trailing whitespace */
	*(end + 1) = 0x00;

	/* If had leading whitespace, then move entire string back to beginning */
	if (s != start)
		memmove(s, start, end - start + 1);

	return;
}

/**
 * Extract next token from string
 * @param line Line to extract token from
 * @param token Pointer to char array where token should be copied
 * @param token_length Length of token array
 * @return Pointer to the start of the following token
 */
static char *command_token(char *line, char *token, int token_length) {

	/* Skip preceding whitespace */
	while (*line && isspace((unsigned char) *line))
		line++;

	/* Copy token */
	while (*line && token_length-- && !isspace((unsigned char) *line))
		*token++ = *line++;
	*token = '\0';

	/* Skip succeeding whitespace */
	while (*line && isspace((unsigned char) *line))
		line++;

	return line;

}

/**
 * Concatenate argument vector to string
 * @param ctx Context with arguments to concat
 * @return Pointer to concatenated arguments
 */
char *command_args(struct command_context *ctx) {
	int i;
	char *p;

	if (ctx->argc > 2) {
		for (i = 1; i < ctx->argc - 1; i++) {
			p = ctx->argv[i];
			p[strlen(p)] = ' ';
		}
	}

	return ctx->argv[1];
}

/**
 * Build argument vector from string
 * @param line String to build argument vector from
 * @param argc Pointer to integer where argument count is stored
 * @param argv Pointer to char pointer array where arguments are stored
 * @return 0 if the vector was successfully build, -1 otherwise
 */
int command_build_argv(char *line, int *argc, char **argv) {

	int escape = CMD_ESCAPE_NONE;

	*argc = 0;
	*argv = (char *) line;

	if (*line == '\0')
		return 0;

	/* We have at least one argument */
	(*argc)++;

	while (*line) {
		/* Single quotes */
		if (*line == '\'') {
			if (escape == CMD_ESCAPE_NONE) {
				escape = CMD_ESCAPE_QUOTE;
				(*argv)++;
			} else if (escape == CMD_ESCAPE_QUOTE) {
				escape = CMD_ESCAPE_NONE;
				*line = '\0';
			}
		}
		/* Double quotes */
		else if (*line == '\"') {
			if (escape == CMD_ESCAPE_NONE) {
				escape = CMD_ESCAPE_DQUOTE;
				(*argv)++;
			} else if (escape == CMD_ESCAPE_DQUOTE) {
				escape = CMD_ESCAPE_NONE;
				*line = '\0';
			}
		}
		/* Whitespace */
		else if (isspace((unsigned char) *line) && escape == CMD_ESCAPE_NONE) {
			/* Delete space */
			*line++ = '\0';

			/* Flush spaces to next token */
			while (*line && isspace((unsigned char) *line))
				line++;

			/* If there is more data, we have another argument */
			if (*line) {
				(*argc)++;
				*++argv = (char *) line;
			}

			continue;
		}

		line++;
	}

	/* Invalid number of escapes */
	if (escape != CMD_ESCAPE_NONE)
		return -1;

	return 0;

}

#if 0
command_t *__command_search(command_t *cmds, int cmd_count, const char *line) {

	int i;
	command_t *cmd, *ret;
	char token[MAX_TOKEN_SIZE];

	const char * next = command_token(line, token, MAX_TOKEN_SIZE);

	for (i = 0; i < cmd_count; i++) {
		cmd = &cmds[i];
		if (cmd_strncmp(token, cmd->name, CMD_MAX_LEN_CMD) == 0) {
			/* Go to next level? */
			if (cmd_read_ptr(&cmd->chain.list)) {
				ret = __command_search(cmd_read_ptr(&cmd->chain.list), cmd_read_int(&cmd->chain.count), next);
				if (ret != NULL)
					return ret;
			}

			return cmd;
		}
	}

	return NULL;

}

command_t *command_search(char *line) {

	/* Return NULL if no commands are registered or line is empty */
	if (!commands || strlen(line) < 1)
		return NULL;

	return __command_search(commands, command_count, line);

}
#endif

/**
 * Parse and execute command
 * @param cmds Command list to execute from
 * @param cmd_count Number of commands in command list
 * @param line Line to parse for command
 * @return CMD_ERROR_NONE if command was parsed, error code if execution failed
 */
static int __command_run(command_t *cmds, int cmd_count, char *line) {

	int ret, i;
	command_t *cmd;
	struct command_context context;
	char token[MAX_TOKEN_SIZE];

	extern int in_pipe_mode;

	if(in_pipe_mode){
		context.pipe_mode=1;
	}
	else{
		context.pipe_mode=0;
	}

	char * next = command_token(line, token, MAX_TOKEN_SIZE);

	for (i = 0; i < cmd_count; i++) {
		cmd = &cmds[i];

		/* Skip no console */
		if (cmd_read_int(&cmd->mode) & CMD_NO_CONSOLE)
			continue;

		if (cmd_strcmp(token, cmd->name) == 0) {
			/* Go to next level? */
			if (cmd_read_ptr(&cmd->chain.list)) {
				ret = __command_run(cmd_read_ptr(&cmd->chain.list), cmd_read_int(&cmd->chain.count), next);
				if (ret != CMD_ERROR_NOTFOUND)
					return ret;
			}

			/* Return error if trying to execute a placeholder command */
			if (!cmd_read_ptr(&cmd->handler)) {
				if (strlen(next))
					return CMD_ERROR_NOTFOUND;
				else
					return CMD_ERROR_CHAINED;
			}
			
			/* Set command backpointer */
			context.command = cmd;

			/* Allocate memory for argument vector */
			char *argv[MAX_ARGC];
			int argc;

			/* Build arguments */
			if (command_build_argv((char *)next, &argc, &argv[1]) != 0)
				return CMD_ERROR_NOMEM;

			context.argc = argc;
			context.argv = argv;

			/* Add command name as first argument */
			context.argv[0] = token;
			context.argc++;

			command_handler_t handler = (void *) cmd_read_ptr(&cmd->handler);
			ret = handler(&context);

			if (ret == CMD_ERROR_SYNTAX) {
#ifdef __AVR__
				printf("usage: ");
				printf_P(cmd->name);
				printf(" ");
				printf_P(cmd->usage);
				printf("\r\n");
#else
				printf("usage: %s %s\r\n", cmd->name, cmd->usage ? : "");
#endif
			}

			return ret;

		}
	}

	return CMD_ERROR_NOTFOUND;

}

/**
 * Parse and run command
 * @param line Line to parse and run
 * @return CMD_ERROR_NONE if parsing succeeded, CMD_ERROR_AMBIGUOUS if multiple
 * commands matches, CMD_ERROR_NOTFOUND if no command matches
 */
int command_run(char *iline) {

	size_t linelength;
	linelength=strlen(iline);
	int i=0;
	int last=0;
	char * line;
	int ret;
	int go=0;

	for(i=0; i<=linelength; i++){
		if(i==linelength){
			line=(char*) calloc(i-last,sizeof(char));
			strncpy(line,&iline[last],i-last);
			go=1;
		}
		else if(i<linelength){
				if(strncmp(&iline[i]," | ",3)==0){
					line=(char*) calloc(i-last,sizeof(char));
					strncpy(line,&iline[last],i-last);
					last=i+3; //beginning of the new line
					go=1;
				}
		}
		if(go==1){
			/*RUN THE COMMAND*/

			/* Return if no commands are registered */
			if (!commands)
				return CMD_ERROR_NOMEM;

			/* Save original string before command_parse mangles it */
			char *org = strdup(line);

			/* Strip leading and ending whitespace, inplace */
			strstrip(org);

			/* Parse and execute command */
			ret = __command_run(commands, command_count, line);
			if (ret != CMD_ERROR_NONE) {
				if (ret == CMD_ERROR_NOTFOUND) {
					printf("Unknown command \'%s\'\r\n", org);
					break;
				} else if (ret == CMD_ERROR_CHAINED) {
					printf("\'%s\' contains sub-commands:\r\n", org);
					char help_str[CONSOLE_BUFSIZ+1];
					sprintf(help_str, "%s ", org);
					command_help(help_str);
					break;
				} else if (ret == CMD_ERROR_INVALID) {
					printf("Could not execute command \'%s\', Invalid argument(s)\r\n", org);
					break;
				} else {
					printf("Could not execute command \'%s\', error %d\r\n", org, ret);
					break;
				}
			}

			/* Restore original string */
			strcpy(line, org);
			free(org);
			free(line);
			go=0;
		}
	}

	return ret;
}

/**
 * Extend command
 * @param cmds Command list to extend from
 * @param cmd_count Number of commands in command list
 * @param line Line to extend
 * @return CMD_ERROR_NONE if command was extended, error code if extension failed
 */
static int __command_complete(const command_t *cmds, int cmd_count, char *line) {

	int i;
	unsigned int token_length, before;
	const command_t * cmd = NULL;
	char token[MAX_TOKEN_SIZE];
	const char * extend = NULL;
	char * next;

	/* Fetch next token */
	next = command_token(line, token, MAX_TOKEN_SIZE);
	token_length = strlen(token);

	//printf("Complete %s %u", next, token_length);

	/* If the token is empty, and there is more than one command in the list */
	if (strlen(token) < 1)
		if (cmd_count != 1)
			return CMD_ERROR_AMBIGUOUS;

	/* Check all commands at this level */
	for (i = 0; i < cmd_count; i++) {
		cmd = &cmds[i];

		/* Skip no console */
		if (cmd_read_int(&cmd->mode) & CMD_NO_CONSOLE)
			continue;

		/* Skip hidden */
		if (cmd_read_int(&cmd->mode) & CMD_HIDDEN)
			continue;

		/* Search for first chars of token and command name */
		if (cmd_strncmp(token, cmd->name, token_length) == 0) {

			/* Continue traversing the chain untill the end of the list */
			if (cmd_read_ptr(&cmd->chain.list) && token_length == cmd_strlen(cmd->name) && strchr(line, ' '))
				return __command_complete(cmd_read_ptr(&cmd->chain.list), cmd_read_int(&cmd->chain.count), (char *) next);

			/* If we have a perfect match, stop searching (and goto completer) */
			if (token_length == cmd_strlen(cmd->name) && strchr(line, ' ')) {
#ifndef __AVR__
				if (!cmd_read_ptr(&cmd->completer))
					extend = cmd->name;
#endif
				break;
			}

			/* Otherwise, search for best extend match */
			if (extend)
				return CMD_ERROR_AMBIGUOUS;
			extend = cmd->name;

		}

	}

	//printf("Next %s %p %s %s\r\n", next, cmd, extend, cmd->name);

	/* Check if this was an extension */
	if (extend) {

		//printf("Now we extend %s with %s\r\n", line, extend);
		/* Fast forward to the word */
		while (*line && isspace((unsigned char) *line)) line++;
		before = strlen(line);
		cmd_strcpy(line, extend);
		strcat(line, " ");
		if (before == strlen(line))
			return CMD_ERROR_COMPLETE;
		else
			return CMD_ERROR_EXTENDED;
	}

	/* Check if we have a completer function for rich-tab completion of commands */
#ifndef __AVR
	if (cmd_read_ptr(&cmd->completer))
		return cmd->completer((char *) next);
#endif

	/* If we don't, then additional chars means there was no match */
	if (strlen(next) > 0)
		return CMD_ERROR_NOTFOUND;

	return CMD_ERROR_NOTFOUND;
}

/**
 * Tab complete command
 * @param line Line to tab complete
 * @return CMD_ERROR_NONE if parsing succeeded, CMD_ERROR_AMBIGUOUS if multiple
 * commands matches, CMD_ERROR_NOTFOUND if no command matches
 */
int command_complete(char *line) {

	/* Return if no commands are registered */
	if (!commands)
		return CMD_ERROR_NOMEM;

	/* If the line is empty, the command is definitely ambiguous */
	if (strlen(line) < 1)
		return CMD_ERROR_AMBIGUOUS;

	return __command_complete(commands, command_count, line);

}

/**
 * List commands
 * @param cmds Command list to list from
 * @param cmd_count Number of commands in command list
 * @param line Line to list
 * @return CMD_ERROR_NONE if list succeeded, error code if listing failed
 */
static int __command_help(const command_t *cmds, int cmd_count, char *line) {

	int i, ret;
	const command_t * cmd;
	char token[MAX_TOKEN_SIZE];
	const char * next;

	next = command_token(line, token, MAX_TOKEN_SIZE);

	if (strnlen(token, MAX_TOKEN_SIZE) > 0) {
		for (i = 0; i < cmd_count; i++) {
			cmd = &cmds[i];
			if (cmd_strncmp(token, cmd->name, CMD_MAX_LEN_CMD) == 0 && strchr(line, ' ')) {
				if (cmd_read_ptr(&cmd->chain.list)) {
					ret = __command_help(cmd_read_ptr(&cmd->chain.list), cmd_read_int(&cmd->chain.count), (char *) next);
					if (ret == CMD_ERROR_NOTFOUND)
						return ret;
				}
			}
		}
	}

	/* Print help for every command that matches */
	for (i = 0; i < cmd_count; i++) {
		cmd = &cmds[i];

		/* Skip no console */
		if (cmd_read_int(&cmd->mode) & CMD_NO_CONSOLE)
			continue;

		/* Skip hidden */
		if (cmd_read_int(&cmd->mode) & CMD_HIDDEN)
			continue;

		if (strlen(token) < 1 || cmd_strncmp(token, cmd->name, strlen(token)) == 0) {
#if defined(__AVR__)
			printf("  ");
			printf_P(cmd->name);
			for (unsigned int i = 0; i < (CMD_MAX_LEN_CMD - strnlen_P(cmd->name, CMD_MAX_LEN_CMD) + 2); i++)
				printf(" ");
			printf_P(cmd->help);
			printf("\r\n");
#else
			printf("  %-19s %s\r\n", cmd->name, cmd->help);
#endif
		}
	}

	return CMD_ERROR_NOTFOUND;

}

/**
 * Show help
 * @param line Line to parse for help
 * @return CMD_ERROR_NONE if parsing succeeded, error code otherwise
 */
int command_help(char *line) {

	/* Return if no commands are registered */
	if (!commands)
		return CMD_ERROR_NOMEM;

	return __command_help(commands, command_count, line);

}

static int __command_usage(const command_t *cmds, int cmd_count, char *line) {

	int i, ret;
	const command_t *cmd;
	char token[MAX_TOKEN_SIZE];
	const char * next;

	next = command_token(line, token, MAX_TOKEN_SIZE);

	for (i = 0; i < cmd_count; i++) {
		cmd = &cmds[i];
		if (cmd_strncmp(token, cmd->name, CMD_MAX_LEN_CMD) == 0 && strchr(line, ' ')) {
			if (cmd_read_ptr(&cmd->chain.list)) {
				ret = __command_usage(cmd_read_ptr(&cmd->chain.list), cmd_read_int(&cmd->chain.count), (char *) next);
				if (ret == CMD_ERROR_NOTFOUND)
					return ret;
			}
		}
	}

	/* Print help for every command that matches */
	for (i = 0; i < cmd_count; i++) {
		cmd = &cmds[i];

		/* Skip no console */
		if (cmd_read_int(&cmd->mode) & CMD_NO_CONSOLE)
			continue;

		/* Skip hidden */
		if (cmd_read_int(&cmd->mode) & CMD_HIDDEN)
			continue;

		if (strlen(token) < 1 || cmd_strncmp(token, cmd->name, strlen(token)) == 0) {
			{
#ifdef __AVR__
				printf("usage: ");
				printf_P(cmd->name);
				printf(" ");
				printf_P(cmd->usage);
				printf("\r\n");
#else
				printf("usage: %s %s\r\n", cmds[i].name, cmds[i].usage ? : "");
#endif
			}
		}
	}

	return CMD_ERROR_NOTFOUND;

}

int command_usage(char *line) {

	/* Return if no commands are registered */
	if (!commands)
		return CMD_ERROR_NOMEM;

	return __command_usage(commands, command_count, line);
}

/**
 * Enable command list.
 * @note This function is still required when linker sections are used
 * to prevent the linker from garbage collecting the seemingly unused
 * command arrays
 * @param cmd Pointer to command array
 * @param cmd_count Number of commands in command array
 * @return CMD_ERROR_NONE if commands were successfully enabled, error code otherwise
 */
int command_enable(command_t * cmd, int cmd_count) {
	if (cmd == NULL)
		return CMD_ERROR_NOMEM;

#ifdef COMMAND_MANUAL_SETUP
	command_lock();
	commands = realloc(commands, (command_count + cmd_count) * sizeof(command_t));

	if (commands == NULL)
		return CMD_ERROR_NOMEM;

	memcpy(&commands[command_count], cmd, cmd_count * sizeof(command_t));
	command_count += cmd_count;
	command_unlock();
#endif

	return CMD_ERROR_NONE;
}

/**
 * Initialize command system
 * @return CMD_ERROR_NONE if initialization succeeded, error code otherwise
 */
int command_init(void) {

#ifndef __linux__
	command_mutex = xSemaphoreCreateMutex();
	if (!command_mutex)
		return CMD_ERROR_NOMEM;
#endif

#ifdef __linux__
	extern volatile unsigned int __start_commands;
	extern volatile unsigned int __stop_commands;
	command_count = ((ptrdiff_t)&__stop_commands - (ptrdiff_t)&__start_commands) / sizeof(command_t);
	commands = (command_t *) &__start_commands;
#else
	if (&__command_start != NULL) {
		command_count = ((ptrdiff_t)&__command_end - (ptrdiff_t)&__command_start) / sizeof(command_t);
		commands = (command_t *) &__command_start;
	}
#endif

	return CMD_ERROR_NONE;
}


