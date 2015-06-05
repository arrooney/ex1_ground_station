/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_STRING_H_
#define PARAM_STRING_H_

#include <param/param_types.h>

/**
 * Search for a specific parameter in a parameter table, based on the name
 * @param table Pointer to table
 * @param table_count Number of elements in table
 * @param name Name to search for
 * @return pointer to parameter or NULL
 */
const param_table_t * param_find_name(const param_table_t table[], size_t table_count, char * name);

/**
 * Search for a specific parameter in a parameter table, based on the address
 * @param table Pointer to table
 * @param table_count Number of elements in table
 * @param addr Address to search for
 * @return pointer to parameter or NULL
 */
const param_table_t * param_find_addr(const param_table_t table[], size_t table_count, uint16_t addr);

/**
 * Print out a single parameter in the same format as param_list
 * @param param pointer to parameter
 * @param mem pointer to memory
 * @param do_read Toggle data readout
 */
void param_list_single(const param_table_t * param, param_index_t * mem, int do_read);

/**
 * Print the contents of an indexed parameter table to stdout
 * @param mem Pointer to indexed parameter table
 * @param do_read Toggle data readout
 */
void param_list(param_index_t * mem, int do_read);

/**
 * Same as param_list, but without addressed and type info.
 * @param mem Pointer to indexed parameter table
 * @param do_read Toggle data readout
 */
void param_list_simple(param_index_t * mem, int do_read);

/**
 * Convert from string to parameter
 * @param param Pointer to parameter description
 * @param string Pointer to string
 * @param value Pointer to memory to put the result
 * @return -1 if error, 0 if ok
 */
int param_from_string(const param_table_t * param , char * string, void * value);

/**
 * Convert from parameter to string
 * @param param pointer to parameter description
 * @param buf Pointer to string
 * @param pos Offset in buffer to put output
 * @param value Pointer to parameter value
 * @param with_type Toggle inclusion of data type in string
 * @param max_size Max size of string
 * @return
 */
int param_to_string(const param_table_t * param, uint8_t * buf, int pos, void * value, int with_type, int max_size);

#endif /* PARAM_STRING_H_ */
