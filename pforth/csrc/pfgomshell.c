#include <pfgomshell.h>
#include <command/command.h>
#include <CCThreadedQueue.h>
#include <csp/csp.h>
#include <csp/csp_cmp.h>
#include <util/timestamp.h>
#include <stdlib.h>
#include <time.h>

#define GOMSHELL_OCP_COMMAND_START "exec 'COMMAND(\""
#define GOMSHELL_OCP_COMMAND_START_LENGTH strlen(GOMSHELL_OCP_COMMAND_START)

#define GOMSHELL_OCP_COMMAND_ARG_DELIM '|'

#define GOMSHELL_OCP_COMMAND_ARG "\", \""
#define GOMSHELL_OCP_COMMAND_ARG_LENGTH strlen(GOMSHELL_OCP_COMMAND_ARG)

#define GOMSHELL_OCP_COMMAND_END "\");'"
#define GOMSHELL_OCP_COMMAND_END_LENGTH strlen(GOMSHELL_OCP_COMMAND_END)

extern struct CCThreadedQueue gomshell_input;

void gomshellErrorOK( void )
{
	PUSH_DATA_STACK(GOMSHELL_OK);
}

void gomshellErrorMem( void )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
}

void gomshellErrorFTP( void )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_FTP);
}

void gomshellErrorSyntax( void )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
}

void gomshellErrorCom( void )
{
	PUSH_DATA_STACK(GOMSHELL_ERR_COM);
}

void gomshellManErrorCodes( void )
{
	sdTerminalPrint("\nGomshell error code meanings:\n"
			"\t %d -- No error / success\n"
			"\t%d -- Failure to allocate memory\n"
			"\t%d -- Failure to complete an FTP command\n",
			"\t%d -- User error (out of bounds parameter or syntax)\n",
			"\t%d -- Unkown communication failure with ExAlta-1\n",
			GOMSHELL_OK, GOMSHELL_ERR_MEM, GOMSHELL_ERR_FTP, GOMSHELL_ERR_SYNTAX, GOMSHELL_ERR_COM);			
}

void gomshellCommandRun( cell_t string, cell_t length )
{
	char* command;
	char exec_char;
	int i;

	command = (char*) string;
	exec_char = '\n';
       
	/* Insert the command into the gomshell's input queue.
	 */
	sdTerminalPrint("\nGomshell command -- ");
	for( i = 0; i < length; ++i ) {
		CCThreadedQueue_Insert(&gomshell_input, &command[i], COS_BLOCK_FOREVER);
	}
	/* Insert '\n' so that the gomshell executes the command.
	 */
	CCThreadedQueue_Insert(&gomshell_input, &exec_char, COS_BLOCK_FOREVER);
}

void gomshellCommand( cell_t string, cell_t length )
{
	gomshellCommandRun(string, length);
}

void gomshellPrintErrorCodes( void )
{
	gomshellManErrorCodes( );
}

void gomshellFtpDownload( cell_t file_name_cell, cell_t file_name_size )
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

void gomshellOCPCommand( cell_t token, cell_t token_length)
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

void gomshellFtpUpload( cell_t file_name, cell_t file_name_length )
{
	return;
}

void gomshellFtpRemove( cell_t file_name_cell, cell_t file_name_cell_length )
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

void gomshellPing( cell_t node_, cell_t timeout_, cell_t size_ )
{
	int node, timeout, size;
	int resp_time;
	csp_timestamp_t start, stop;

	node = (unsigned int) node_;
	timeout = (unsigned int) timeout_;
	size = (unsigned int) size_;
	
	sdTerminalPrint("Ping name %d, timeout %d, size %d: ", node, timeout, size);

	clock_get_time(&start);
	resp_time = csp_ping(node, timeout, size, CSP_O_NONE);
	clock_get_time(&stop);

	timestamp_diff((timestamp_t*) &stop, (timestamp_t*) &start);
	if (resp_time < 0) {
		sdTerminalPrint("Timeout after %.03f ms\r\n", stop.tv_sec * 1000 + stop.tv_nsec / 1E6);
		PUSH_DATA_STACK(GOMSHELL_ERR_COM);
	} else {
		sdTerminalPrint("Reply in %.03f ms\r\n", stop.tv_sec * 1000 + stop.tv_nsec / 1E6);
		PUSH_DATA_STACK(GOMSHELL_OK);
	}
}

