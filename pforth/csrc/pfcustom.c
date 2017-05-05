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
#include <CCThreadedQueue.h>
#ifdef GOMSHELL
#include <command/command.h>
extern const char* console_get_prompt_identifier( );
extern size_t console_get_prompt_identifier_length( );
#define GOMSHELL_OUTPUT_TIMEOUT 15*1000
#endif

#define EOR_BUFFER_SIZE 33

static cell_t CTest0( cell_t Val );
static void CTest1( cell_t Val1, cell_t Val2 );

/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/
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

#ifdef GOMSHELL
static void gomshellCommand( cell_t string, cell_t length )
{
	int i;
	IOHook_Printf_FP print_fp;
	struct CCThreadedQueue* gomshell_input;
	struct CCThreadedQueue* gomshell_output;
	CCTQueueError err;
	char* command;
	const char* eor; /* eor: end of response. */
	char response;
	char exec_char;
	size_t eor_match_size;

	print_fp = IOHook_GetPrintf( );
	gomshell_input = IOHook_GetGomshellInputQueue( );
	gomshell_output = IOHook_GetGomshellOutputQueue( );
	command = (char*) string;
	exec_char = '\n';
	eor = console_get_prompt_identifier( );
	eor_match_size = console_get_prompt_identifier_length( );
	
	print_fp("\nGomshell command -- ");

	/* Clear output queue to remove old data.
	 */
	CCThreadedQueue_Clear(gomshell_output);

	/* Insert the command into the gomshell's input queue.
	 */
	for( i = 0; i < length; ++i ) {
		CCThreadedQueue_Insert(gomshell_input, &command[i], COS_BLOCK_FOREVER);
	}
	/* Insert '\n' so that the gomshell executes the command.
	 */
	CCThreadedQueue_Insert(gomshell_input, &exec_char, COS_BLOCK_FOREVER);

	/* Read the response from the gomshell's output queue.
	 */
	for( i = 0; ; ) {
		err = CCThreadedQueue_Remove(gomshell_output, &response, COS_BLOCK_FOREVER);
		/* Error check response.
		 */
		if( err == CCTQUEUE_OK ) {
			print_fp("%c", response);
		}
		else if( err == CCTQUEUE_ERR_TIMEOUT ) {
			print_fp("gomshell - timeout waiting for input\n");
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
	print_fp("\n");	
}

static void ftpDownload( cell_t file_name_cell, cell_t file_name_size )
{
	extern int cmd_ftp_download_file(struct command_context *);
	struct command_context command;
	char* argv[2];
	char* file_name;

	/* Copy name of file to be downloaded into a
	 * NULL terminated string.
	 */
	file_name = malloc(file_name_size+1);
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
}

#else
static void gomshellCommand( cell_t string, cell_t length )
{
	return;
}

static void ftpDownload( cell_t file_name )
{

}

#endif



#include <stdlib.h>
static void programExit( )
{
	exit(EXIT_SUCCESS);
}

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
    (CFunc0) programExit
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
** Make sure order of functions matches that in LoadCustomFunctionTable().
** Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
    err = CreateGlueToC( "CTEST0", i++, C_RETURNS_VALUE, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "CTEST1", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "FIN", i++, C_RETURNS_VOID, 0 );
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

