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
extern const char* console_get_prompt_identifier( );
extern size_t console_get_prompt_identifier_length( );
#endif
#define GOMSHELL_OUTPUT_TIMEOUT 15*1000

#define GOMSHELL_OK		0
#define GOMSHELL_ERR_MEM	-1
#define GOMSHELL_ERR_FTP	-2

#define GOMSHELL_OCP_COMMAND_START "exec 'COMMAND(\""
#define GOMSHELL_OCP_COMMAND_START_LENGTH strlen(GOMSHELL_OCP_COMMAND_START)

#define GOMSHELL_OCP_COMMAND_END "\");'"
#define GOMSHELL_OCP_COMMAND_END_LENGTH strlen(GOMSHELL_OCP_COMMAND_END)

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

static void gomshellManErrorCodes( )
{
	sdTerminalPrint("\nGomshell error code meanings:\n"
			"\t %d -- No error / success\n"
			"\t%d -- Failure to allocate memory\n"
			"\t%d -- Failure to complete an FTP command\n",
			GOMSHELL_OK, GOMSHELL_ERR_MEM, GOMSHELL_ERR_FTP);			
}

/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/
static void gomshellCommand( cell_t string, cell_t length )
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

static void gomshellOCPCommand( cell_t command, cell_t command_length )
{
	char* command_string;
	char* command_start = GOMSHELL_OCP_COMMAND_START;
	size_t command_start_length = GOMSHELL_OCP_COMMAND_START_LENGTH;
	char* command_end = GOMSHELL_OCP_COMMAND_END;
	size_t command_end_length = GOMSHELL_OCP_COMMAND_END_LENGTH;
	size_t total_length = command_start_length + command_length + command_end_length;
	size_t i;

	command_string = malloc(total_length);
	if( command_string == NULL ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
		return;
	}

	strncpy(command_string, command_start, command_start_length);
	strncpy(command_string + command_start_length, command, command_length);
	strncpy(command_string + command_start_length + command_length, command_end, command_end_length);
	
/*	sdTerminalPrint("\nOCP Command -- %.*s", total_length, command_string);*/
	gomshellCommand((cell_t) command_string, (cell_t) total_length);
}

static void gomshellOCPLongCommand( cell_t arg1, cell_t arg2, cell_t arg3, cell_t arg4 )
{
	return;
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

static void gomshellOCPLongCommand( cell_t arg1, cell_t arg2, cell_t arg3, cell_t arg4 )
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
    (CFunc0) gomshellOCPLongCommand
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
    err = CreateGlueToC( "GOM.LCOMMAND", i++, C_RETURNS_VOID, 4 );
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

