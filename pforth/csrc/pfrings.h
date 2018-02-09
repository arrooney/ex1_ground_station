#ifndef PF_RINGS_WORDS
#define PF_RINGS_WORDS

#include <stdlib.h>
#include <CCArrayList.h>
#include <CCListIterator.h>
#include <CCThreadedQueue.h>
#include "pf_all.h"

/* Different ring buffer identifiers.
 * Do not change. These are NOT arbitrary.
 */
#define GOMSHELL_RING_WOD 	4
#define GOMSHELL_RING_DFGM_RAW	2
#define GOMSHELL_RING_DFGM_S0	0
#define GOMSHELL_RING_DFGM_S1	1
#define GOMSHELL_RING_DFGM_HK	3
#define GOMSHELL_RING_ATHENA	5
#define GOMSHELL_RING_MAX_ID 	5

#define GOMSHELL_TOTAL_RINGS 6
#define GOMSHELL_RING_STRING_LENGTH 3*60
#define GOMSHELL_RING_NAME_LENGTH 12
#define LOGGER_TOTAL_SEQUENCE_BYTES 3
#define LOGGER_SEQUENCE_START 0
#define LOGGER_SEQUENCE_BASE 16


void pf_uitoa( unsigned int num, char* str, size_t len, int base );
unsigned int pf_atoui( const char* str, size_t len, int base );
unsigned int pf_powerof( unsigned int base, unsigned int exp );
void gomshellRingAdd( struct CIList* tail_list, char* tail_file_name);
CBool gomshellExtractRingTails( struct CIList* ring_string, struct CIList* ring_name );

/**
 * FORTH
 * Puts ID for WOD ring on stack
 */
void gomshellRingWod( void );

/**
 * FORTH
 * Puts ID for raw DFGM ring on stack
 */
void gomshellRingDFGMRaw( void );

/**
 * FORTH
 * Puts ID for S0 DFGM  ring on stack
 */
void gomshellRingDFGMS0( void );

/**
 * FORTH
 * Puts ID for S1 DFGM ring on stack
 */
void gomshellRingDFGMS1( void );

/**
 * FORTH
 * Puts ID for DFGM HK ring on stack
 */
void gomshellRingDFGMHK( void );

/**
 * FORTH
 * Puts ID for Athena ring on stack
 */
void gomshellRingAthena( void );

/**
 * Prints all letters in the list to the terminal.
 */
void gomshellPrintRingName( struct CIList* ring_name );

/**
 * Increments the ring buffer file name given to have
 * the name of the next ring buffer file. This function
 * assumes inifinte ring buckets.
 */
void pf_next_name( char* );

/**
 * Same as pf_next_name() but goes backwards one file.
 */
void pf_previous_name( char* );

/**
 * Same as pf_next_name() but operates on a list
 * structure instead of char array.
 */
void gomshellRingIncrementName( struct CIList* list_name );

/**
 * Same as pf_previous_name() but operates on a list
 * structure instead of char array.
 */
void gomshellRingDecrementName( struct CIList* list_name );

/**
 * FORTH
 * Download ring buffer pointers based on ring ID.
 */
void gomshellFetchRingByID( cell_t id );

/**
 * FORTH
 * Increment/decrement ring buffer file name a given number
 * of times.
 */
void gomshellRingMove( cell_t id, cell_t position, cell_t direction );

/**
 * FORTH
 * Use OCP telecommand to fetch all ring buffer names.
 * This is very slow and glitchy. Use gomshellFetchRingByID()
 * instead.
 */
void gomshellRingFetch( void );

/**
 * FORTH
 * Download the tail file of a ring buffer based on it's
 * ID. Assumes the ring buffer meta data has been fetched
 * with gomshellFetchRingByID() first.
 */
void gomshellRingDownload( cell_t ring_index );

/**
 * FORTH
 * Same as pf_next_name but callable within forth interpreter.
 */
void incrementRingName( cell_t string_, cell_t length_ );


#endif
