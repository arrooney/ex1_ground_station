#ifndef PF_GOMSHELL_WORDS
#define PF_GOMSHELL_WORDS

#include "pf_all.h"

/* Result codes for some gomshell functions
 */
#define GOMSHELL_OK		0
#define GOMSHELL_ERR_MEM	-1
#define GOMSHELL_ERR_FTP	-2
#define GOMSHELL_ERR_SYNTAX	-3
#define GOMSHELL_ERR_COM	-4
#define GOMSHELL_OUTPUT_TIMEOUT 25*1000

/** 
 * FORTH
 * Pings the specified node in the CSP network.
 */
void gomshellPing( cell_t node );

/**
 * FORTH
 * Puts ID for no error in gomshell function on stack.
 */
void gomshellErrorOK( void );

/**
 * FORTH
 * Puts ID for memory error  in gomshell function on stack.
 * Memory errors are due to bad mallocs, uninitiallized 
 * pointers, etc.
 */
void gomshellErrorMem( void );

/**
 * FORTH
 * Puts ID for FTP error in gomshell function on stack.
 */
void gomshellErrorFTP( void );

/**
 * FORTH
 * Puts ID for syntax in gomshell function on stack.
 * This error is due to incorrect syntactical structure
 * of a gomshell function.
 */
void gomshellErrorSyntax( void );

/**
 * FORTH
 * Puts ID for COM error in gomshell function on stack.
 * This error is due to a problem in the space-ground communications
 * link that prevented succesful use of the gomshell function.
 */
void gomshellErrorCom( void );

/** 
 * Print description of all gomshell error codes to terminal.
 */
void gomshellManErrorCodes( void );

/**
 * Executes the gomshell command given.
 */
void gomshellCommandRun( cell_t string, cell_t length );

/**
 * FORTH
 * Calls gomshellCommandRun() then gomshellFlushOutput().
 */ 
void gomshellCommand( cell_t string, cell_t length );

/**
 * FORTH 
 * Print description of all gomshell error codes to terminal.
 */
void gomshellPrintErrorCodes( void );

/**
 * FORTH
 * Downloads the file. File path must be included. ie, /sd/ or /boot/
 * Puts error code on stack.
 */
void gomshellFtpDownload( cell_t file_name_cell, cell_t file_name_size );

/**
 * FORTH
 * OCP style telecommand. Sytax is:
 * S" telecommand|argument"
 * Puts error code on stack. This error only indicates if the command
 * was successfully sent. It does not indicate successful onboard execution.
 */
void gomshellOCPCommand( cell_t token, cell_t token_length);

/**
 * FORTH
 * Not implemented yet. Uploads a file.
 * Puts error code on stack.
 */
void gomshellFtpUpload( cell_t file_name, cell_t file_name_length );

/**
 * FORTH
 * Remove files from S/C memory. Must include file path.
 * Puts error code on stack.
 */
void gomshellFtpRemove( cell_t file_name_cell, cell_t file_name_cell_length );

#endif
