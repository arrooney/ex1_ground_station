#ifndef PF_UNIX_WORDS
#define PF_UNIX_WORDS

#include "pf_all.h"

/**
 * FORTH
 * Runs a terminal command using the C system() function
 */
void unixShellCommand( cell_t cmd_string_, cell_t length_ );

/**
 * FORTH
 * Blocks the calling process until the unix time given
 */
void processBlock( cell_t wake_time_ );

/**
 * FORTH
 * Blocks the calling process foer the given number of
 * seconds.
 */
void processSleep( cell_t sleep_time_ );

/**
 * FORTH
 * Kills the program (everything, not just the forth interpreter).
 */
void programExit( void );

/**
 * FORTH
 * Same as forth word TYPE but with a color parameter
 * color =
 * 1: red
 * 2: yellow
 * 3: green
 * 4: blue
 * 5: magenta
 * 6: grey
 * 7: default
 */
void colorType( cell_t addr, cell_t len, cell_t color );

#endif
