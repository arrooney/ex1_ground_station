/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <rparam_server.h>
#include <param/param_types.h>
#include <param/param_serializer.h>
#include <param_index.h>
#include <param_store.h>
#include <param_host.h>

void rparam_service_handler(csp_conn_t * conn, csp_packet_t * request_packet) {

	rparam_query * request;
	csp_packet_t * reply_packet = NULL;
	rparam_query * reply;

	/* Handle endian */
	request = (rparam_query *) request_packet->data;
	request->length = csp_ntoh16(request->length);

	/* Check valid mem */
	param_index_t * mem = param_ptr(request->mem);
	if (mem == NULL) {
		csp_buffer_free(request_packet);
		return;
	}

	/* Calculate checksum on table */
	uint16_t rparam_checksum = param_index_chksum(mem);

	switch(request->action) {
	case RPARAM_GET: {

		if (request->length > RPARAM_QUERY_MAX_LEN) {
			printf("Request too long %u\r\n", request->length);
			goto out_free;
		}

		/* Deal with endianness */
		for (unsigned int i = 0; i < request->length/2; i++)
			request->addr[i] = csp_ntoh16(request->addr[i]);
		request->checksum = csp_ntoh16(request->checksum);

		/* Check checksum */
		if ((request->checksum != rparam_checksum) && (request->checksum != 0xB00B)) {
			printf("Invalid table checksum\r\n");
			goto out_free;
		}

		unsigned int length, start = 0;

		/* Save memory:
		 * With full table requests we dont need the request packet no more */
		unsigned int type = 0;
		if (request->length == 0) {
			type = 1;
			csp_buffer_free(request_packet);
			request_packet = NULL;
		}

		while (start + 1 <= mem->count) {
			reply_packet = csp_buffer_get(RPARAM_QUERY_MAX_LEN);
			if (reply_packet == NULL)
				goto out_free;

			reply = (rparam_query *) reply_packet->data;
			reply->action = RPARAM_REPLY;

			/* Get data */
			if (type == 1) {
				length = param_serialize_full_table(mem, &start, reply->packed, RPARAM_QUERY_MAX_LEN, F_TO_BIG_ENDIAN);
			} else {
				length = param_serialize_list(mem, request->addr, request->length/2, &start, reply->packed, RPARAM_QUERY_MAX_LEN, F_TO_BIG_ENDIAN);
			}

			/* If we did not serialize anything, abort */
			if (length <= 0) {
				printf("Serializer fail\r\n");
				csp_buffer_free(reply_packet);
				goto out_free;
			}

			/* Set sequence */
			reply->seq = csp_hton16(start);
			reply->total = csp_hton16(mem->count);

			/* Set lengths */
			reply->length = csp_hton16(length);
			reply_packet->length = length + offsetof(rparam_query, packed);

			if (!csp_send(conn, reply_packet, 0)) {
				csp_buffer_free(reply_packet);
				goto out_free;
			}
		}

		goto out_free;
	}
	case RPARAM_SET: {
		/* Check checksum */
		request->checksum = csp_ntoh16(request->checksum);
		if ((request->checksum != rparam_checksum) && (request->checksum != 0xB00B)) {
			request_packet->data[0] = RPARAM_ERROR;
			goto out_send_status;
		}

		if (param_deserialize(mem, request->packed, request->length, F_FROM_BIG_ENDIAN) == 0) {
			request_packet->data[0] = RPARAM_SET_OK;
		} else {
			request_packet->data[0] = RPARAM_ERROR;
		}
		goto out_send_status;
	}
	case RPARAM_TABLE: {
#ifdef __AVR__
		csp_sfp_send_own_memcpy(conn, (void *) mem->table, mem->count * sizeof(param_table_t), RPARAM_QUERY_MAX_LEN, 10000, memcpy_P);
#else
		csp_sfp_send(conn, (void *) mem->table, mem->count * sizeof(param_table_t), RPARAM_QUERY_MAX_LEN, 10000);
#endif

		goto out_free;
	}
	case RPARAM_COPY: {
		param_copy(param_ptr(request->copy.from), param_ptr(request->copy.to), 0);
		request_packet->data[0] = RPARAM_COPY_OK;
		goto out_send_status;
	}
	case RPARAM_LOAD: {
		if (param_load(request->copy.from, param_ptr(request->copy.to)) == 0) {
			request_packet->data[0] = RPARAM_LOAD_OK;
		} else {
			request_packet->data[0] = RPARAM_ERROR;
		}
		goto out_send_status;
	}
	case RPARAM_SAVE: {
		if (param_save(param_ptr(request->copy.from), request->copy.to) == 0) {
			request_packet->data[0] = RPARAM_SAVE_OK;
		} else {
			request_packet->data[0] = RPARAM_ERROR;
		}
		goto out_send_status;
	}
	case RPARAM_CLEAR: {
		param_clear(request->clear.id);
		request_packet->data[0] = RPARAM_CLEAR_OK;
		goto out_send_status;
	}
	default:
		request_packet->data[0] = RPARAM_ERROR;
		goto out_send_status;
	}

out_send_status:
	request_packet->length = 1;
	if (csp_send(conn, request_packet, 0))
		return;
out_free:
	if (request_packet != NULL)
		csp_buffer_free(request_packet);

}
