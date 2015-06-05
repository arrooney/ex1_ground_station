/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_SERIALIZER_H_
#define PARAM_SERIALIZER_H_

#include <param/param_types.h>

/**
 * Serializer / Deserializer flags
 */
typedef enum {
	F_DRY_RUN           = (1 << 0), //!< F_DRY_RUN do not write to memory
	F_TO_BIG_ENDIAN     = (1 << 1), //!< F_TO_BIG_ENDIAN Convert from host to big endian
	F_FROM_BIG_ENDIAN   = (1 << 2),	//!< F_FROM_BIG_ENDIAN Confert from big endian to host order
	F_PACKED            = (1 << 3),	//!< F_PACKED Do not pack addresses
	F_PRINT_GET         = (1 << 4), //!< F_PRINT_GET Print GET
	F_PRINT_SET         = (1 << 5), //!< F_PRINT_SET Print SET
	F_PRINT_REP         = (1 << 6), //!< F_PRINT_REP Print REP
} param_serializer_flags;

/**
 * In-place conversion of a single parameter from big endian to host byte order
 * @param type param type
 * @param item pointer to parameter memory
 * @return 1 if memory has been swapped, 0 if not
 */
int param_betoh(param_type_t type, void * item);

/**
 * In-place conversion of a single parameter from host byte order to big endian
 * @param type param type
 * @param item porinter to parameter memory
 * @return 1 if memory has been swapped, 0 if not
 */
int param_htobe(param_type_t type, void * item);

/**
 * Serialize data
 * @param mem Pointer to indexed parameter table
 * @param start Pointer to iterator integer, or NULL if no iterator is needed
 * @param buf Pointer to output
 * @param maxbuflen Max output buffer length
 * @param flags Set options using combination of flags
 * @return -1 if err, 0 if ok
 */
int param_serialize_full_table(param_index_t * mem, unsigned int *start, uint8_t * buf, unsigned int maxbuflen, param_serializer_flags flags);

/**
 * Serialize data
 * @param mem Pointer to indexed parameter table
 * @param addr Array of addresses to serialize
 * @param addr_count number of items in addr array
 * @param start Pointer to iterator integer
 * @param buf Pointer to output
 * @param maxbuflen Max output buffer length
 * @param flags Set options using combination of flags
 * @return -1 if err, 0 if ok
 */
int param_serialize_list(param_index_t * mem, const uint16_t addr[], unsigned int addr_count, unsigned int *start, uint8_t * buf, unsigned int maxbuflen, param_serializer_flags flags);

/**
 * Serialize all elements in a parameter array
 * @param mem Pointer to indexed parameter table
 * @param param Pointer to parameter to serialize
 * @param buf Pointer to output
 * @param pos Pointer to output position
 * @param maxlen Max output buffer length
 * @param flags Set options using combination of flags
 * @return -1 if err, 0 if ok
 */
int param_serialize_array(param_index_t * mem, const param_table_t * param, uint8_t * buf, uint16_t * pos, unsigned int maxlen, param_serializer_flags flags);

/**
 * Serialize single item
 * @param param Pointer to specific parameter to serialize
 * @param addr Address of item
 * @param buf Pointer to output
 * @param pos Position in output to use
 * @param maxlen Max output buffer length
 * @param item Pointer to memory where the value should be taken from
 * @param flags Set options using combination of flags
 * @return -1 if err, 0 if ok
 */
int param_serialize_item(const param_table_t * param, uint16_t addr, uint8_t * buf, uint16_t * pos, unsigned int maxlen, void * item, param_serializer_flags flags);

/**
 * Deserialize a string of packed parameters into memory
 * @param mem Pointer to indexed parameter table to use
 * @param buf Pointer to serialized data
 * @param len Length of serialized data
 * @param flags Set options using combination of flags
 * @return -1 if err, 0 if ok
 */
int param_deserialize(param_index_t * mem, uint8_t * buf, int len, param_serializer_flags flags);

/**
 * Deserialize a specific item from a string into memory
 * @param param Pointer to specific parameter to deserialize
 * @param addr Address of parameter
 * @param mem Pointer to indexed parameter table to use
 * @param item Pointer to memory area where item should be written
 * @param flags Set options using combination of flags
 * @return -1 if err, 0 if ok
 */
int param_deserialize_item(const param_table_t * param, uint16_t addr, param_index_t * mem, void * item, param_serializer_flags flags);

#endif /* PARAM_SERIALIZER_H_ */
