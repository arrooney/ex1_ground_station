/**
 * RAM store header
 *
 * @author Morten Jensen and Johan De Claville Christiansen
 * Copyright 2012 GomSpace ApS. All rights reserved.
 */

#ifndef RAM_STORE_H_
#define RAM_STORE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <csp/csp.h>

#define BLOB_MAX_PATH_LEN	50
#define BLOB_MAX_NAME_LEN	10

#define BLOB_FLAG_COMPRESSED_MASK	(1 << 4)

typedef struct blob_t {
	struct blob_t *	next_blob;
	uint8_t * 		mem_addr;
	uint32_t		mem_size;
	uint32_t		mem_used;
	uint32_t		creation_time;
	uint16_t		user_flags;
	bool			compressed;
	char			name[BLOB_MAX_NAME_LEN];
} blob_t;


struct __attribute__((__packed__)) blob_list {
	uint8_t  count;				//! Number in packet
	uint8_t  end_flag;			//! This is the last packet;
	struct __attribute__((__packed__)) {
		uint16_t id;			//! The idendtifier is the number in the index
		uint32_t mem_addr;		//! Memory address of data
		uint32_t mem_used;		//! Actually used size in blob
		uint32_t mem_size;		//! The size allocated in mem
		uint32_t creation_time;		//! Time of creation
		uint16_t user_flags;		//! Arbitrary user flags
		uint8_t name[9];		//! "shortname" to help remember
	} blobs[7];
};

/**
 * Create a new blob
 * @param size wanted blob size in bytes
 * @param user_flags arbitraty flags
 * @param name blob name
 * @return blob if ok else NULL
 */
blob_t * blob_create(uint32_t size, uint16_t user_flags, char *name);

/**
 * Destroy blob
 * @param blob pointer to blob
 */
void blob_destroy(blob_t *blob);

/**
 * Write data to blob
 * @param blob pointer to blob
 * @param data data to write
 * @param len number of bytes to write
 * @return 1 if ok
 */
int blob_write(blob_t *blob, void *data, uint32_t len);

/**
 * Get blob at index
 * @param index index of wanted blob
 * @return pointer to blob if ok else NULL
 */
blob_t * blob_get(unsigned int index);

/**
 * Print list of blobs in memory
 */
void blob_print_list(void);

/**
 * Compress blob image
 * After this, please do not write to the buffer
 * @param blob pointer to blob
 * @return <0 if err
 */
int blob_compress(blob_t *blob);

/**
 * Store blob to file
 * @param blob blob to be stored
 * @param path
 * @return 1 if ok
 */
int blob_sink_fat(blob_t *blob, char *path);

/**
 * Store blob to file
 * @param blob blob to be stored
 * @param path
 * @return 1 if ok
 */
int blob_sink_file(blob_t *blob, const char *path);

/**
 * Service to be used by server
 * @param conn pointer to conn
 * @param packet pointer to packet
 */
void service_blob_ls(csp_conn_t * conn, csp_packet_t * packet);

/**
 * Client
 */
void blob_set_server(uint8_t node, uint8_t port);
int blob_ls(void);

#endif /* RAM_STORE_H_ */
