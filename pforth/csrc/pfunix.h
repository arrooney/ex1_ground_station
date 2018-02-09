#ifndef PF_UNIX_WORDS
#define PF_UNIX_WORDS

#include "pf_all.h"

/**
 * Runs a terminal command using the C system() function
 */
void unixShellCommand( cell_t cmd_string_, cell_t length_ );

/**
 * Blocks the calling process until the unix time given
 */
void processBlock( cell_t wake_time_ );

/**
 * Blocks the calling process foer the given number of
 * seconds.
 */
void processSleep( cell_t sleep_time_ );

/**
 * Kills the program (everything, not just the forth interpreter).
 */
void programExit( void );

#endif
