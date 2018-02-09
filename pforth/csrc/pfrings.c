#ifdef GOMSHELL

#include "pfrings.h"
#include "pf_all.h"
#include "pfgomshell.h"

static struct CCArrayList ring_tails_backend[GOMSHELL_TOTAL_RINGS];
static struct CCArrayList ring_tails;

static struct CCArrayList ring_heads_backend[GOMSHELL_TOTAL_RINGS];
static struct CCArrayList ring_heads;

void gomshellRingWod( void )
{
	PUSH_DATA_STACK(GOMSHELL_RING_WOD);
}

void gomshellRingDFGMRaw( void )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_RAW);
}

void gomshellRingDFGMS0( void )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_S0);
}

void gomshellRingDFGMS1( void )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_S1);
}

void gomshellRingDFGMHK( void )
{
	PUSH_DATA_STACK(GOMSHELL_RING_DFGM_HK);
}

void gomshellRingAthena( void )
{
	PUSH_DATA_STACK(GOMSHELL_RING_ATHENA);
}

static void gomshellRingInit( void )
{
	static CBool ring_init = CFALSE;
	int i;

	if( !ring_init ) {
 		CCArrayList(&ring_tails, sizeof(struct CIList*), GOMSHELL_TOTAL_RINGS);
		CCArrayList(&ring_heads, sizeof(struct CIList*), GOMSHELL_TOTAL_RINGS);
		
		for( i = 0; i < GOMSHELL_TOTAL_RINGS; ++i ) {
			CCArrayList(&ring_tails_backend[i], sizeof(char), GOMSHELL_RING_NAME_LENGTH);
			CCArrayList(&ring_heads_backend[i], sizeof(char), GOMSHELL_RING_NAME_LENGTH);

			struct CIList* list_location = &(ring_tails_backend[i]).cilist;
			CIList_Add(&ring_tails.cilist, &list_location);

			list_location = &(ring_heads_backend[i]).cilist;
			CIList_Add(&ring_heads.cilist, &list_location);

		}
		ring_init = CTRUE;
	}
}


void pf_next_name( char* name )
{
	unsigned int next_seq = 0;
	unsigned int next_tem = 0;

	/* Increment temporal and sequence number. */
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

unsigned int pf_powerof( unsigned int base, unsigned int exp )
{
  unsigned int i;
  unsigned int num = 1;
  for( i = 0; i < exp; ++i ) {
    num = num * base;
  }
  return num;
}

void pf_uitoa( unsigned int num, char* str, size_t len, int base )
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

unsigned int pf_atoui( const char* str, size_t len, int base )
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

void pf_previous_name( char* name )
{
	unsigned int prev_seq = 0;
	unsigned int prev_tem = 0;

	/* Decrement temporal and sequence number. 
	 */
	prev_seq = (pf_atoui(name + LOGGER_SEQUENCE_START, LOGGER_TOTAL_SEQUENCE_BYTES, LOGGER_SEQUENCE_BASE) - 1);
	prev_tem = (((name[4]-'0')*1000) + ((name[5]-'0')*100) + ((name[6]-'0')*10) + (name[7]-'0') - 1);


	pf_uitoa(prev_seq, name + LOGGER_SEQUENCE_START, LOGGER_TOTAL_SEQUENCE_BYTES, LOGGER_SEQUENCE_BASE);

	name[7] = (char) ((prev_tem % 10) + '0') & 0xFF;
	prev_tem = (prev_tem - (prev_tem % 10)) / 10; /* basically a base ten logical shift right. */
	name[6] = (char) ((prev_tem % 10) + '0') & 0xFF;
	prev_tem = (prev_tem - (prev_tem % 10)) / 10;
	name[5] = (char) ((prev_tem % 10) + '0') & 0xFF;
	prev_tem = (prev_tem - (prev_tem % 10)) / 10;
	name[4] = (char) ((prev_tem % 10) + '0') & 0xFF;

}

void gomshellPrintRingName( struct CIList* ring_name )
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

void gomshellRingIncrementName( struct CIList* list_name )
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

void gomshellRingDecrementName( struct CIList* list_name )
{
	char array_name[GOMSHELL_RING_NAME_LENGTH];
	size_t i;

	for( i = 0; i < CIList_Size(list_name); ++i ) {
		CIList_Get(list_name, &array_name[i], i);
	}
	pf_previous_name(array_name);
	for( i = 0; i < CIList_Size(list_name); ++i ) {
		CIList_AddAt(list_name, &array_name[i], i);
	}
}

void gomshellRingAdd( struct CIList* tail_list, char* tail_file_name)
{
	size_t i;
	
	CIList_Clear(tail_list);
	for( i = 0; i < GOMSHELL_RING_NAME_LENGTH; ++i ) {
		CIList_Add(tail_list, &tail_file_name[i]);
	}
}

CBool gomshellExtractRingTails( struct CIList* ring_string, struct CIList* ring_name )
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

void gomshellFetchRingByID( cell_t id )
{
	cell_t forth_err;
	struct CIList* tail_list;
	struct CIList* head_list;
	size_t bytes_read;
	char tail_file_name[GOMSHELL_RING_NAME_LENGTH+1];
	char head_file_name[GOMSHELL_RING_NAME_LENGTH+1];
	FILE* meta_file;
	static const char* ring_meta_data_path[] = { "/sd/MT-ALog.txt", /* DFGM FIR stream A */
					     "/sd/MT-BLog.txt", /* DFGM FIR stream B */
					     "/sd/MT-RLog.txt", /* DFGM raw stream */
					     "/sd/MT-HLog.txt", /* DFGM HK stream */
					     "/sd/MT-WLog.txt", /* WOD stream */
					     "/sd/MT-TLog.txt" };  /* Athena stream */

	/* Initialize ring lists.
	 */
	gomshellRingInit( );

	if( id > GOMSHELL_RING_MAX_ID || id < 0 ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
		return;
	}

	/* Download the meta data file for the ring of interest.
	 */
      	gomshellFtpDownload((cell_t) ring_meta_data_path[id], (cell_t) strlen(ring_meta_data_path[id]));
	forth_err = POP_DATA_STACK;
	if( forth_err != GOMSHELL_OK ) {
		sdTerminalPrint("error: ftp download");
		PUSH_DATA_STACK(forth_err);
		return;
	}

	/* Open the meta data file and extract the head and tail of the ring buffer.
	 */	
	meta_file = fopen(ring_meta_data_path[id] + strlen("/sd/"), "r");
	if( meta_file == NULL ) {
		sdTerminalPrint("error: fopen");
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
		return;
	}

	bytes_read = fread(head_file_name, sizeof(char), GOMSHELL_RING_NAME_LENGTH+1, meta_file);
	if( bytes_read != GOMSHELL_RING_NAME_LENGTH+1 ) {
		fclose(meta_file);
		sdTerminalPrint("error: fread head");
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
	}
	bytes_read = fread(tail_file_name, sizeof(char), GOMSHELL_RING_NAME_LENGTH+1, meta_file);
	fclose(meta_file);
	if( bytes_read != GOMSHELL_RING_NAME_LENGTH+1 ) {
		sdTerminalPrint("error: fread tail");
		PUSH_DATA_STACK(GOMSHELL_ERR_MEM);
	}
	
	/* Get the correct list to insert the head and tail file names into.
	 */
	CIList_Get(&ring_tails.cilist, &tail_list, (size_t) id);
	CIList_Get(&ring_heads.cilist, &head_list, (size_t) id);

	/* Insert them into the list.
	 */
	gomshellRingAdd(tail_list, tail_file_name);
	gomshellRingAdd(head_list, head_file_name);

	/* Print to user the names.
	 */
	sdTerminalPrint("tail: ");
	gomshellPrintRingName(tail_list);
	sdTerminalPrint("head: ");
	gomshellPrintRingName(head_list);

	PUSH_DATA_STACK(GOMSHELL_OK);
}

void gomshellRingMove( cell_t id, cell_t position, cell_t direction )
{
	struct CIList* file_name;
	CIListError err;

	/* Initialize ring buffers.
	 */
	gomshellRingInit( );
	
	/* Use id to index the ring files buffer
	 */
	if( position == 1 ) {
		err = CIList_Get(&ring_heads.cilist, &file_name, id);
	}
	else if( position == -1 ) {
		err = CIList_Get(&ring_tails.cilist, &file_name, id);
	}
	else {
		PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
		return;
	}

	/* Error check.
	 */
	if( err != CILIST_OK ) {
		PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
		return;
	}
	
	/* Increment or decrement file in buffer.
	 */
	if( direction == 1 ) {
		gomshellRingIncrementName(file_name);
	}
	else if( direction == -1 ) {
		gomshellRingDecrementName(file_name);
	}
	else {
		PUSH_DATA_STACK(GOMSHELL_ERR_SYNTAX);
		return;
	}

	sdTerminalPrint("\nNew name: ");
	gomshellPrintRingName(file_name);

	PUSH_DATA_STACK(GOMSHELL_OK);
	return;

}

void gomshellRingFetch( void )
{
	sdTerminalPrint("Deprecated Function. Go sit in the corner\n");
	PUSH_DATA_STACK(GOMSHELL_OK);
}

void gomshellRingDownload( cell_t ring_index )
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
	err = CIList_Get(&ring_tails.cilist, &list_name, ring_index);
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
		/* Increment tail file in buffer.
	 	*/
		gomshellRingIncrementName(list_name);
	}
	else {
		/* Failed to download. Push error onto stack.
		 */
		PUSH_DATA_STACK(forth_err);
	}
	return;
}

void incrementRingName( cell_t string_, cell_t length_ )
{
	char* string = (char*) string_;
	size_t length = (size_t) length_;

	if( length == 12 ) {
		pf_next_name(string);
	}

	PUSH_DATA_STACK((cell_t) string);
	PUSH_DATA_STACK(length_);
}


#endif
