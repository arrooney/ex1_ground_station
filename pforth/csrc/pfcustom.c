/* @(#) pfcustom.c 98/01/26 1.3 */

#ifndef PF_USER_CUSTOM

/***************************************************************
** Call Custom Functions for pForth
**
** Create a file similar to this and compile it into pForth
** by setting -DPF_USER_CUSTOM="mycustom.c"
**
** Using this, you could, for example, call X11 from Forth.
** See "pf_cglue.c" for more information.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**
***************************************************************/


#include "pf_all.h"
#include <IOHook.h>
#include <stdlib.h>
#ifdef GOMSHELL
#include <CCThreadedQueue.h>
#include <command/command.h>
#include <CCArrayList.h>
#include <CCListIterator.h>
#include <slre.h>
extern const char* console_get_prompt_identifier( );
extern size_t console_get_prompt_identifier_length( );
#endif

#define MAX_BOOT_BYTES 512

#define GOMSHELL_OUTPUT_TIMEOUT 15*1000
#define GOMSHELL_TOTAL_RINGS 6
#define GOMSHELL_RING_STRING_LENGTH 3*60
#define GOMSHELL_RING_NAME_LENGTH 12

#define LOGGER_TOTAL_SEQUENCE_BYTES 3
#define LOGGER_SEQUENCE_START 0
#define LOGGER_SEQUENCE_BASE 16

/* Result codes for some gomshell functions
 */
#define GOMSHELL_OK		0
#define GOMSHELL_ERR_MEM	-1
#define GOMSHELL_ERR_FTP	-2
#define GOMSHELL_ERR_SYNTAX	-3
#define GOMSHELL_ERR_COM	-4

/* Different ring buffer identifiers.
 * Do not change. These are NOT arbitrary.
 */
#define GOMSHELL_RING_WOD 	4
#define GOMSHELL_RING_DFGM_RAW	2
#define GOMSHELL_RING_DFGM_S0	0
#define GOMSHELL_RING_DFGM_S1	1
#define GOMSHELL_RING_DFGM_HK	3
#define GOMSHELL_RING_ATHENA	5

#define GOMSHELL_OCP_COMMAND_START "exec 'COMMAND(\""
#define GOMSHELL_OCP_COMMAND_START_LENGTH strlen(GOMSHELL_OCP_COMMAND_START)

#define GOMSHELL_OCP_COMMAND_ARG_DELIM '|'

#define GOMSHELL_OCP_COMMAND_ARG "\", \""
#define GOMSHELL_OCP_COMMAND_ARG_LENGTH strlen(GOMSHELL_OCP_COMMAND_ARG)

#define GOMSHELL_OCP_COMMAND_END "\");'"
#define GOMSHELL_OCP_COMMAND_END_LENGTH strlen(GOMSHELL_OCP_COMMAND_END)

#ifdef GOMSHELL
static struct CCArrayList ring_names_backend[GOMSHELL_TOTAL_RINGS];
static struct CCArrayList ring_names;
#endif

static cell_t CTest0( cell_t Val );
static void CTest1( cell_t Val1, cell_t Val2 );

/****************************************************************/
/* Helper functions for gomshell words.				*/
/****************************************************************/
#ifdef GOMSHELL
static void gomshellErrorOK( )
{
	PUSH_DATA_STACK(GOMSHELL_OK);
}

static void gomshellErrorMem( )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
}

static void gomshellErrorFTP( )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_FTP);
}

static void gomshellErrorSyntax( )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
}

static void gomshellErrorCom( )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_COM);
}

static unsigned int pf_powerof( unsigned int base, unsigned int exp )
{
  unsigned int i;
  unsigned int num = 1;
  for( i = 0; i < exp; ++i ) {
    num = num * base;
  }
  return num;
}

static void pf_uitoa( unsigned int num, char* str, size_t len, int base )
{
  size_t i;
  unsigned int rem;
  char ascii_num;

  /* Zero fill string. */
  for( i = 0; i < len; ++i ) {
    str[i] = '0';
  }

  /* Check for zero. */
  if( num == 0 ) {
    return;
  }

  for( i = 0; i < len; ++i ) {
    rem = num % base;
    ascii_num = (rem > 9) ? ((rem-10) + 'a') : (rem + '0');
    str[len-i-1] = ascii_num;
    num = (num - rem) / base;
  }
}

static unsigned int pf_atoui( const char* str, size_t len, int base )
{
  size_t i;
  unsigned int num = 0;
  unsigned int inter_num = 0;


  for( i = 0; i < len; ++i ) {
    /* Convert ascii byte to int. */
    if( str[i] >= '0' && str[i] <= '9' ) {
      inter_num = str[i] - '0';
    }
    else if( str[i] >= 'a' && str[i] <= 'z' ) {
      inter_num = str[i] - 'a' + 10;
    }
    else if( str[i] >= 'A' && str[i] <= 'Z' ) {
      inter_num = str[i] - 'A' + 10;
    }
    else {
      return 0;
    }

    /* accumulate. */
    num += inter_num * pf_powerof(base, len - i - 1);
  }

  return num;
}

static void pf_next_name( char* );
static void gomshellRingIncrementName( struct CIList* list_name )
{
	char array_name[GOMSHELL_RING_NAME_LENGTH];
	size_t i;

	for( i = 0; i < CIList_Size(list_name); ++i ) {
		CIList_Get(list_name, &array_name[i], i);
	}
	pf_next_name(array_name);
	for( i = 0; i < CIList_Size(list_name); ++i ) {
		CIList_AddAt(list_name, &array_name[i], i);
	}
}

static void pf_next_name( char* name )
{
	unsigned int next_seq = 0;
	unsigned int next_tem = 0;

	/* Increment temporal and sequence number, then roll over if necessary. */
	next_seq = (pf_atoui(name + LOGGER_SEQUENCE_START, LOGGER_TOTAL_SEQUENCE_BYTES, LOGGER_SEQUENCE_BASE) + 1);
	next_tem = (((name[4]-'0')*1000) + ((name[5]-'0')*100) + ((name[6]-'0')*10) + (name[7]-'0') + 1);


	pf_uitoa(next_seq, name + LOGGER_SEQUENCE_START, LOGGER_TOTAL_SEQUENCE_BYTES, LOGGER_SEQUENCE_BASE);

	name[7] = (char) ((next_tem % 10) + '0') & 0xFF;
	next_tem = (next_tem - (next_tem % 10)) / 10; /* basically a base ten logical shift right. */
	name[6] = (char) ((next_tem % 10) + '0') & 0xFF;
	next_tem = (next_tem - (next_tem % 10)) / 10;
	name[5] = (char) ((next_tem % 10) + '0') & 0xFF;
	next_tem = (next_tem - (next_tem % 10)) / 10;
	name[4] = (char) ((next_tem % 10) + '0') & 0xFF;
}

static void gomshellPrintRingName( struct CIList* ring_name )
{
	struct CCListIterator iter;
	char msg;
	
	CCListIterator(&iter, ring_name);
	while( CIIterator_HasNext(&iter.ciiterator) ) {
		CIIterator_Next(&iter.ciiterator, &msg);
		sdTerminalOut(msg);
	}
	sdTerminalOut('\n');
	CDestroy(&iter);
}

static CBool gomshellExtractRingTails( struct CIList* ring_string, struct CIList* ring_name )
{
	struct CCListIterator iter;
	char msg;
	const char* id = ".bin";
	const size_t id_length = strlen(id);
	size_t i;

	CCListIterator(&iter, ring_string);
	i = 0;

	while( CIIterator_HasNext(&iter.ciiterator) ) {
		CIIterator_Next(&iter.ciiterator, &msg);
		if( msg == id[i++] ) {
			if( i == id_length ) {
				CIList_Clear(ring_name);
				for( i = 0; i < GOMSHELL_RING_NAME_LENGTH; ++ i ) {
					CIIterator_Previous(&iter.ciiterator, &msg);
					CIList_AddAt(ring_name, &msg, GOMSHELL_RING_NAME_LENGTH-1-i);
				}
				CDestroy(&iter);
				return CTRUE;
			}
		}
		else {
			i = 0;
		}
	}
	CDestroy(&iter);
	return CFALSE;
}

static void gomshellFlushOutput( )
{
	CCTQueueError err;
	struct CCThreadedQueue* gomshell_output;
	const char* eor; /* eor: end of response. */
	size_t eor_match_size;
	int i;
	char response;
	
	eor = console_get_prompt_identifier( );
	eor_match_size = console_get_prompt_identifier_length( );
	gomshell_output = IOHook_GetGomshellOutputQueue( );

	for( i = 0; ; ) {
		err = CCThreadedQueue_Remove(gomshell_output, &response, COS_BLOCK_FOREVER);
		/* Error check response.
		 */
		if( err == CCTQUEUE_OK ) {
			sdTerminalOut(response);
		}
		else if( err == CCTQUEUE_ERR_TIMEOUT ) {
			sdTerminalPrint("gomshell - timeout waiting for input\n");
			break;
		}
		else {
			break;
		}

		/* Check for multiple character end of response identifier
		 * This is the gomshell prompt:
		 *	csp-term #
		 */
		if( response == eor[i] ) {
			++i;
			if( i == eor_match_size) {
				break;
			}
		}
		else {
			i = 0;
		}
	}
	sdTerminalOut('\n');
}

void gomshellRingInit( )
{
	static CBool ring_names_init = CFALSE;
	int i;

	if( !ring_names_init ) {
 		CCArrayList(&ring_names, sizeof(struct CIList*), GOMSHELL_TOTAL_RINGS);
		for( i = 0; i < GOMSHELL_TOTAL_RINGS; ++i ) {
			CCArrayList(&ring_names_backend[i], sizeof(char), GOMSHELL_RING_NAME_LENGTH);

			struct CIList* list_location = &(ring_names_backend[i]).cilist;
			CIList_Add(&ring_names.cilist, &list_location);
		}
		ring_names_init = CTRUE;
	}
}

static void gomshellManErrorCodes( )
{
	sdTerminalPrint("\nGomshell error code meanings:\n"
			"\t %d -- No error / success\n"
			"\t%d -- Failure to allocate memory\n"
			"\t%d -- Failure to complete an FTP command\n",
			"\t%d -- User error (out of bounds parameter or syntax)\n",
			"\t%d -- Unkown communication failure with ExAlta-1\n",
			GOMSHELL_OK, GOMSHELL_ERR_MEM, GOMSHELL_ERR_FTP, GOMSHELL_ERR_SYNTAX, GOMSHELL_ERR_COM);			
}

/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/

static void gomshellCommandRun( cell_t string, cell_t length )
{
	struct CCThreadedQueue* gomshell_input;
	struct CCThreadedQueue* gomshell_output;
	char* command;
	char exec_char;
	int i;

	gomshell_output = IOHook_GetGomshellOutputQueue( );
	gomshell_input = IOHook_GetGomshellInputQueue( );
	command = (char*) string;
	exec_char = '\n';
       
	/* Clear output queue to remove old data.
	 */
	CCThreadedQueue_Clear(gomshell_output);

	/* Insert the command into the gomshell's input queue.
	 */
	sdTerminalPrint("\nGomshell command -- ");
	for( i = 0; i < length; ++i ) {
		CCThreadedQueue_Insert(gomshell_input, &command[i], COS_BLOCK_FOREVER);
	}
	/* Insert '\n' so that the gomshell executes the command.
	 */
	CCThreadedQueue_Insert(gomshell_input, &exec_char, COS_BLOCK_FOREVER);
}

static void gomshellCommand( cell_t string, cell_t length )
{
	gomshellCommandRun(string, length);
	
	/* Read the response from the gomshell's output queue.
	 */
	gomshellFlushOutput( );
}

static void gomshellPrintErrorCodes( )
{
	gomshellManErrorCodes( );
}

static void gomshellFtpDownload( cell_t file_name_cell, cell_t file_name_size )
{
	extern int cmd_ftp_download_file(struct command_context *);
	struct command_context command;
	char* argv[2];
	char* file_name;

	/* Give user a message.
	 */
	sdTerminalPrint("\nGomshell ftp download --\n");
	
	/* Copy name of file to be downloaded into a
	 * NULL terminated string.
	 */
	file_name = malloc(file_name_size+1);
	if( file_name == NULL ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
		return;
	}
	strncpy(file_name, (char*) file_name_cell, file_name_size);
	file_name[file_name_size] = '\0';

	/* Setup command context for Gomshell to download
	 * the file.
	 */
	command.argv = argv;
	command.argv[0] = "download_file";
	command.argv[1] = file_name;
	command.argc = 2;

	/* Do gomshell command that will download the file.
	 */
	int result = cmd_ftp_download_file(&command);
	if( result != CMD_ERROR_NONE ) {
		/* Failed to download.
		 */
		PUSH_DATA_STACK(GOMSHELL_ERR_FTP);
	}
	else {
		/* Success.
		 */
		PUSH_DATA_STACK(GOMSHELL_OK);
	}

	free(file_name);
}

static void gomshellOCPCommand( cell_t token, cell_t token_length)
{
	size_t i;
	size_t ocp_arg_delim_index;
	size_t ocp_copy_offset;
	size_t ocp_command_length;
	char* ocp_command;
	size_t ocp_command_name_length;
	char* ocp_command_name;
	size_t ocp_command_arg_length;
	char* ocp_command_arg;
	
	/* Search the token for the option argument deliminator.
	 */
	ocp_arg_delim_index = 0;
	for( i = 0; i < token_length; ++i ) {
		if( ((char*) token)[i] == GOMSHELL_OCP_COMMAND_ARG_DELIM) {
			ocp_arg_delim_index = i;
			break;
		}
	}

	/* Calculate total length of OCP command string.
	 */
	if( ocp_arg_delim_index > 0 ) {
		ocp_command_name_length = ocp_arg_delim_index;
		ocp_command_name = (char*) token;

		ocp_command_arg_length = token_length - ocp_arg_delim_index - 1;
		ocp_command_arg = ((char*) token) + ocp_arg_delim_index + 1;
		
		ocp_command_length =
			GOMSHELL_OCP_COMMAND_START_LENGTH +	/* Command start deliminator. */
			ocp_command_name_length +		/* Command name. */
			GOMSHELL_OCP_COMMAND_ARG_LENGTH +	/* Deliminator between argument name and command name. */
			ocp_command_arg_length +		/* Argument name. */
			GOMSHELL_OCP_COMMAND_END_LENGTH;	/* Command end deliminator. */
	}
	else {
		ocp_command_name_length = token_length;
		ocp_command_name = (char*) token;
		
		ocp_command_length =
			GOMSHELL_OCP_COMMAND_START_LENGTH +	/* Command start deliminator. */
			token_length +				/* command name. */
			GOMSHELL_OCP_COMMAND_END_LENGTH;	/* Command end deliminator. */
	}
	
	/* Allocate memory for OCP command string.
	 */
	ocp_command = malloc(ocp_command_length);
	if( ocp_command == NULL ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
		return;
	}
	PUSH_DATA_STACK(GOMSHELL_OK);

	/* Begin forming the ocp command string.
	 * This contains the beginning of command deliminator, followed by the command name.
	 */
	ocp_copy_offset = 0;
	strncpy(ocp_command + ocp_copy_offset,
		GOMSHELL_OCP_COMMAND_START,
		GOMSHELL_OCP_COMMAND_START_LENGTH);
	ocp_copy_offset += GOMSHELL_OCP_COMMAND_START_LENGTH;
	
	strncpy(ocp_command + ocp_copy_offset,
		ocp_command_name,
		ocp_command_name_length);
	ocp_copy_offset += ocp_command_name_length;

	if( ocp_arg_delim_index > 0 ) {
		/* Add the optional argument deliminator and option argument name
		 * to ocp command string.
		 */
		strncpy(ocp_command + ocp_copy_offset,
			GOMSHELL_OCP_COMMAND_ARG,
			GOMSHELL_OCP_COMMAND_ARG_LENGTH);
		ocp_copy_offset += GOMSHELL_OCP_COMMAND_ARG_LENGTH;
		
		strncpy(ocp_command + ocp_copy_offset,
			ocp_command_arg,
			ocp_command_arg_length);
		ocp_copy_offset += ocp_command_arg_length;
	}
		
	strncpy(ocp_command + ocp_copy_offset,
		GOMSHELL_OCP_COMMAND_END,
		GOMSHELL_OCP_COMMAND_END_LENGTH);
	ocp_copy_offset += GOMSHELL_OCP_COMMAND_END_LENGTH;

	/* Send the string to the gomshell for execution. 
	 * Note, at this point, ocpy_copy_offset and ocp_command_length are the same.
	 */
	gomshellCommand((cell_t) ocp_command, (cell_t) ocp_command_length);
	free(ocp_command);
}

static void gomshellFtpUpload( cell_t file_name, cell_t file_name_length )
{
	return;
}

static void gomshellFtpRemove( cell_t file_name_cell, cell_t file_name_cell_length )
{
	extern int cmd_ftp_remove(struct command_context *);
	struct command_context command;
	char* argv[2];
	char* file_name;

	/* Give user a message.
	 */
	sdTerminalPrint("\nGomshell ftp remove -- ");
	
	/* Copy name of file to be removed into a
	 * NULL terminated string.
	 */
	file_name = malloc(file_name_cell_length+1);
	if( file_name == NULL ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
		return;
	}
	strncpy(file_name, (char*) file_name_cell, file_name_cell_length);
	file_name[file_name_cell_length] = '\0';
	sdTerminalPrint("%s\n", file_name);
	
	/* Setup command context for Gomshell to remove
	 * the file.
	 */
	command.argv = argv;
	command.argv[0] = "remove_file";
	command.argv[1] = file_name;
	command.argc = 2;

	/* Do gomshell command that will remove the file.
	 */
	int result = cmd_ftp_remove(&command);
	if( result != CMD_ERROR_NONE ) {
		/* Failed to remove.
		 */
		PUSH_DATA_STACK(GOMSHELL_ERR_FTP);
	}
	else {
		/* Success.
		 */
		PUSH_DATA_STACK(GOMSHELL_OK);
	}

	free(file_name);
}

static void gomshellRingFetch( )
{
	const char* ocp_ring_fetch = "downlink";
	const size_t ocp_ring_fetch_length = strlen(ocp_ring_fetch);
	cell_t forth_err;
	char csp_eot;
	char msg;
	int i;
	struct CCThreadedQueue* csp_queue;
	CCTQueueError queue_err;
	struct CCArrayList ring_string;

	gomshellRingInit( );
	
	/* Initialize buffer to hold ring string.
	 */
	CCArrayList(&ring_string, sizeof(char), GOMSHELL_RING_STRING_LENGTH);
	
	/* Need to switch nanomind print output
	 * to a queue that this function can capture
	 */
	extern void CSPPrintSetToTerminal( );
	extern void CSPPrintSetToQueue( );
	CSPPrintSetToQueue( );

	/* Get the print queue used to buffer data from csp_printf( ).
	 * Clear the queue of any transient garbage that may have
	 * came from previous errors.
	 */
	extern struct CCThreadedQueue* CSPPrintGetQueue( );
	csp_queue = CSPPrintGetQueue( );
	CCThreadedQueue_Clear(csp_queue);

	
	/* Need to issue the ocp command 'downlink'
	 */
	gomshellOCPCommand((cell_t) ocp_ring_fetch, (cell_t) ocp_ring_fetch_length);
	forth_err = POP_DATA_STACK;
	if( forth_err != GOMSHELL_OK ) {
		PUSH_DATA_STACK(forth_err);
		return;
	}

	/* Wait for six eot identifiers.
	 */
	extern char CSPPrintGetEOT( );
	csp_eot = CSPPrintGetEOT( );
	for( i = 0; i < GOMSHELL_TOTAL_RINGS; ++i ) {
		CIList_Clear(&ring_string.cilist);
		do {
			queue_err = CCThreadedQueue_Remove(csp_queue, &msg, GOMSHELL_OUTPUT_TIMEOUT);
			if( queue_err != CCTQUEUE_OK ) {
				PUSH_DATA_STACK(GOMSHELL_ERR_COM);
				CDestroy(&ring_string);
				/* Switch nanomind print output back to console.
				 */
				CSPPrintSetToTerminal( );
				return;
			}
			CIList_Add(&ring_string.cilist, &msg);
		} while( msg != csp_eot );

		/* Record the name of the tail file in a buffer
		 */
		struct CIList* name;
		CIList_Get(&ring_names.cilist, &name, i);
		if( gomshellExtractRingTails(&ring_string.cilist, name) ) {
			gomshellPrintRingName(name);
		}
	}

	/* Switch nanomind print output back to console.
	 */
	CSPPrintSetToTerminal( );

	CDestroy(&ring_string);
	PUSH_DATA_STACK(GOMSHELL_OK);
}

static void gomshellRingDownload( cell_t ring_index )
{
	struct CIList* list_name;
	char* array_name;
	const char* sd_dir = "/sd/";
	size_t sd_dir_length = strlen(sd_dir);
	CIListError err;
	cell_t forth_err;
	size_t i;

	gomshellRingInit( );
	
	/* Use ring_name to index the tail files buffer
	 */
	err = CIList_Get(&ring_names.cilist, &list_name, ring_index);
	if( err != CILIST_OK ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
		return;
	}

	/* Copy name into a standard string array.
	 */
	array_name = malloc(sizeof(char) * (CIList_Size(list_name) + sd_dir_length));
	if( array_name == NULL ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
		return;
	}	
	for( i = 0; i < sd_dir_length; ++i ) {
		array_name[i] = sd_dir[i];
	}
	for( ;  i < sd_dir_length + CIList_Size(list_name); ++i ) {
		CIList_Get(list_name, &array_name[i], i - sd_dir_length);
	}
	
	/* Download this file.
	 */
	sdTerminalPrint("Downloading: %.*s\n", CIList_Size(list_name) + sd_dir_length, array_name);
	gomshellFtpDownload((cell_t) array_name, CIList_Size(list_name) + sd_dir_length);
	forth_err = POP_DATA_STACK;

	if( forth_err == GOMSHELL_OK ) {
		/* Download is successful, remove it from nanomind's
		 * memory.
		 * Note, this function puts an error code on the stack. Let that be returning
		 * error code.
		 */
		gomshellFtpRemove((cell_t) array_name, (cell_t) CIList_Size(list_name) + sd_dir_length);
	}
	else {
		/* Failed to download. Push error onto stack.
		 */
		PUSH_DATA_STACK(forth_err);
	}
	
	/* Increment tail file in buffer.
	 */
	gomshellRingIncrementName(list_name);
	return;
}

static void gomshellRingWod( )
{
	PUSH_DATA_STACK(GOMSHELL_RING_WOD);
}

static void gomshellRingDFGMRaw( )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_RAW);
}

static void gomshellRingDFGMS0( )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_S0);
}

static void gomshellRingDFGMS1( )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_S1);
}

static void gomshellRingDFGMHK( )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_HK);
}

static void gomshellRingAthena( )
{
	PUSH_DATA_STACK(GOMSHELL_RING_ATHENA);
}

static void gomshellMnlpDownload( cell_t num_files )      
{
	struct mnlp_file
	{
		char name[GOMSHELL_RING_NAME_LENGTH];
		CBool is_downloaded;
	};
	
	const char* ls_boot = "ftp ls /boot/";
	const size_t ls_boot_len = strlen(ls_boot);
	CCTQueueError err;
	struct CCThreadedQueue* gomshell_output;
	const char* eor; /* eor: end of response. */
	size_t eor_match_size;
	int i, j;
	char response[MAX_BOOT_BYTES];
	size_t response_length;
	const char* regexp = "([M|m][0-9][0-9][0-9][0-9][0-9][0-9][H|S|E|D|h|s|e|d].bin)";
	struct slre_cap match;
	size_t total_files;
	struct mnlp_file mnlp_files[MAX_BOOT_BYTES/GOMSHELL_RING_NAME_LENGTH+1];
	size_t mnlp_files_length = MAX_BOOT_BYTES/GOMSHELL_RING_NAME_LENGTH+1;
	
	/* Capture output of listing contents in /boot/.
	 */
	gomshellCommandRun((cell_t) ls_boot, ls_boot_len);
	eor = console_get_prompt_identifier( );
	eor_match_size = console_get_prompt_identifier_length( );
	gomshell_output = IOHook_GetGomshellOutputQueue( );

	for( i = 0, j = 0; j < MAX_BOOT_BYTES; ) {
		err = CCThreadedQueue_Remove(gomshell_output, &response[j++], COS_BLOCK_FOREVER);
		/* Error check response.
		 */
		if( err == CCTQUEUE_OK ) {
			sdTerminalOut(response);
		}
		else {
			sdTerminalPrint("gomshell - error waiting for input\n");
			PUSH_DATA_STACK(GOMSHELL_ERR_FTP);
			return;
		}
		
		/* Check for multiple character end of response identifier
		 * This is the gomshell prompt:
		 *	csp-term #
		 */
		if( response[j-1] == eor[i] ) {
			++i;
			if( i == eor_match_size) {
				break;
			}
		}
		else {
			i = 0;
		}
	}
	response_length = j;
	
	/* Using regexp, extract all MnLP file names.
	 */
	for( total_files = 0, j = 0, i = 1;
	     j < response_length && i > 0 && total_files < mnlp_files_length;
	     ++total_files )
	{
		i = slre_match(regexp, response + j, response_length - j, &match, 1, 0);
		if( i < 0 ) {
			break;
		}
		sdTerminalPrint("Found file: [%.*s]\n", match.len, match.ptr);
		strncpy(mnlp_files[total_files].name, match.ptr, GOMSHELL_RING_NAME_LENGTH);
		mnlp_files[total_files].is_downloaded = CFALSE;
		j += i;
	}

	/* Switch to backend 1.
	 */

	/* Download num_files worth of MnLP data from /sd/
	 * Log all files the failed to download.
	 */

	/* Switch to backend 2.
	 */

	/* Loop through all captures files from /boot/
	 */

	/* If the files was downloaded successfully, remove it 
	 * from /sd/ THEN /boot/ (this order is important! why?
	 * Because we capture files from /boot/. If the file does
	 * not exist in /boot/ but exists in /sd/, then it will 
	 * never get removed.
	 */

	/* If the file did not download successfully, download
	 * it from /boot/.
	 */

	/* If successful, remove it from /sd/ THEN /boot/.
	 */
}

#endif

static void programExit( )
{
	exit(EXIT_SUCCESS);
}

static cell_t CTest0( cell_t Val )
{
    MSG_NUM_D("CTest0: Val = ", Val);
    return Val+1;
}

static void CTest1( cell_t Val1, cell_t Val2 )
{

    MSG("CTest1: Val1 = "); ffDot(Val1);
    MSG_NUM_D(", Val2 = ", Val2);
}

#ifndef GOMSHELL
static void gomshellMnlpDownload( cell_t num_files )
{
	return;
}

static void gomshellCommand( cell_t arg1, cell_t arg2 )
{
	return;
}

static void gomshellPrintErrorCodes( )
{
	return;
}

static void gomshellFtpDownload( cell_t arg1, cell_t arg2 )
{
	return;
}

static void gomshellErrorOK( )
{
	return;
}

static void gomshellErrorMem( )
{
	return;
}

static void gomshellErrorFTP( )
{
	return;
}

static void gomshellOCPCommand( cell_t arg1, cell_t arg2 )
{
	return;
}

static void gomshellErrorSyntax( )
{
	return;
}

static void gomshellErrorCom( )
{
	return;
}

static void gomshellFtpUpload( cell_t file_name, cell_t file_name_length )
{
	return;
}

static void gomshellFtpRemove( cell_t file_name, cell_t file_name_length )
{
	return;
}

static void gomshellRingDownload( cell_t ring_name )
{
	
}

static void gomshellRingWod( )
{
	return;
}

static void gomshellRingDFGMRaw( )
{
	return;
}

static void gomshellRingDFGMS0( )
{
	return;
}

static void gomshellRingDFGMS1( )
{
	return;
}

static void gomshellRingDFGMHK( )
{
	return;
}

static void gomshellRingAthena( )
{
	return;
}

static void gomshellRingFetch( )
{
	return;
}	

#endif       

/****************************************************************
** Step 2: Create CustomFunctionTable.
**     Do not change the name of CustomFunctionTable!
**     It is used by the pForth kernel.
****************************************************************/

#ifdef PF_NO_GLOBAL_INIT
/******************
** If your loader does not support global initialization, then you
** must define PF_NO_GLOBAL_INIT and provide a function to fill
** the table. Some embedded system loaders require this!
** Do not change the name of LoadCustomFunctionTable()!
** It is called by the pForth kernel.
*/
#define NUM_CUSTOM_FUNCTIONS  (2)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = CTest0;
    CustomFunctionTable[1] = CTest1;
    return 0;
}

#else
/******************
** If your loader supports global initialization (most do.) then just
** create the table like this.
*/
CFunc0 CustomFunctionTable[] =
{
    (CFunc0) CTest0,
    (CFunc0) CTest1,
    (CFunc0) gomshellCommand,
    (CFunc0) programExit,
    (CFunc0) gomshellPrintErrorCodes,
    (CFunc0) gomshellFtpDownload,
    (CFunc0) gomshellErrorOK,
    (CFunc0) gomshellErrorMem,
    (CFunc0) gomshellErrorFTP,
    (CFunc0) gomshellOCPCommand,
    (CFunc0) gomshellErrorSyntax,
    (CFunc0) gomshellRingDownload,
    (CFunc0) gomshellRingWod,
    (CFunc0) gomshellRingDFGMRaw,
    (CFunc0) gomshellRingDFGMS0,
    (CFunc0) gomshellRingDFGMS1,
    (CFunc0) gomshellRingDFGMHK,
    (CFunc0) gomshellRingAthena,
    (CFunc0) gomshellFtpUpload,
    (CFunc0) gomshellErrorCom,
    (CFunc0) gomshellRingFetch,
    (CFunc0) gomshellFtpRemove,
    (CFunc0) gomshellFtpUpload,
    (CFunc0) gomshellMnlpDownload
};
#endif

/****************************************************************
** Step 3: Add custom functions to the dictionary.
**     Do not change the name of CompileCustomFunctions!
**     It is called by the pForth kernel.
****************************************************************/

#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))
Err CompileCustomFunctions( void )
{
    Err err;
    int i = 0;
    
/* Compile Forth words that call your custom functions.
   Make sure order of functions matches that in LoadCustomFunctionTable().
   Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
    err = CreateGlueToC( "CTEST0", i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "CTEST1", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "FIN", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.HELP", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.DOWNLOAD", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.OK", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.MEM", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.FTP", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.COMMAND", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.USER", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DOWNLOAD", i++, C_RETURNS_VOID, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.WOD", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-RAW", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-S0", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-S1", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-HK", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.ATHENA", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.UPLOAD", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.COM", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.FETCH", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.REMOVE", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.UPLOAD", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.MNLP.DOWNLOAD", i++, C_RETURNS_VOID, 1 );
    if( err < 0 ) return err;
        
    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

/****************************************************************
** Step 4: Recompile using compiler option PF_USER_CUSTOM
**         and link with your code.
**         Then rebuild the Forth using "pforth -i system.fth"
**         Test:   10 Ctest0 ( should print message then '11' )
****************************************************************/

#endif  /* PF_USER_CUSTOM */

