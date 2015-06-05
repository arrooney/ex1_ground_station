/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#include <stdio.h>
#include <stddef.h>
#include <alloca.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>

#include <param/param_types.h>
#include <param/param_serializer.h>

int rparam_get_full_table(param_index_t * mem, int node, int port, int mem_id, int timeout) {

	int ret = 0;
	csp_conn_t *conn;
	csp_packet_t *request, *reply;

	request = csp_buffer_get(RPARAM_QUERY_MAX_LEN);
	if (request == NULL)
		return -1;

	conn = csp_connect(CSP_PRIO_HIGH, node, port, timeout, CSP_O_NONE);
	if (!conn) {
		csp_buffer_free(request);
		return -1;
	}

	rparam_query * query = (void *) request->data;
	query->action = RPARAM_GET;
	query->checksum = csp_hton16(0xB00B);
	query->length = csp_hton16(0);
	query->mem = mem_id;

	request->length = query->length + offsetof(rparam_query, addr);

	if (!csp_send(conn, request, timeout)) {
		csp_buffer_free(request);
		csp_close(conn);
		return -1;
	}

	while ((reply = csp_read(conn, timeout)) != NULL) {
		/* We have a reply */
		query = (void *) reply->data;
		query->length = csp_ntoh16(query->length);

		query->seq = csp_ntoh16(query->seq);
		query->total = csp_ntoh16(query->total);

		if (query->action == RPARAM_REPLY)
			param_deserialize(mem, query->packed, query->length, F_FROM_BIG_ENDIAN);

		csp_buffer_free(reply);

		if (query->seq >= query->total)
			break;
	}

	if (!reply)
		ret = -1;

	csp_close(conn);

	return ret;
}

int rparam_get_single(void * out, uint16_t addr, param_type_t type, int size, int mem_id, int node, int port, int timeout) {

	/* Calculate length */
	size_t query_size = offsetof(rparam_query, packed) + sizeof(uint16_t);
	size_t reply_size = offsetof(rparam_query, packed) + size + sizeof(uint16_t);

	rparam_query * query = alloca(reply_size);
	query->action = RPARAM_GET;
	query->mem = mem_id;

	/* Add to query */
	query->addr[0] = csp_hton16(addr);
	query->checksum = csp_hton16(0xB00B);
	query->length = csp_hton16(sizeof(query->addr[0]));

	/* Run single packet transaction */
	if (csp_transaction(CSP_PRIO_HIGH, node, port, timeout, query, query_size, query, reply_size) <= 0)
		return -1;

	/* We have a reply */
	query->length = csp_ntoh16(query->length);

	if (query->length != sizeof(uint16_t) + size) {
		printf("Invalid reply size %u\r\n", query->length);
		return -1;
	}

	/* Read address */
	query->addr[0] = csp_betoh16(query->addr[0]);
	if (query->addr[0] != addr) {
		printf("Invalid address in reply %u\r\n", query->addr[0]);
		return -1;
	}

	/* Read value */
	memcpy(out, &query->packed[2], size);
	param_betoh(type, out);

	return size;

}


uint8_t rparam_get_uint8(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_UINT8, sizeof(uint8_t), mem_id, node, port, timeout);
}
uint16_t rparam_get_uint16(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_UINT16, sizeof(uint16_t), mem_id, node, port, timeout);
}
uint32_t rparam_get_uint32(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_UINT32, sizeof(uint32_t), mem_id, node, port, timeout);
}
uint64_t rparam_get_uint64(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_UINT64, sizeof(uint64_t), mem_id, node, port, timeout);
}
int8_t rparam_get_int8(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_INT8, sizeof(int8_t), mem_id, node, port, timeout);
}
int16_t rparam_get_int16(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_INT16, sizeof(int16_t), mem_id, node, port, timeout);
}
int32_t rparam_get_int32(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_INT32, sizeof(int32_t), mem_id, node, port, timeout);
}
int64_t rparam_get_int64(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_INT64, sizeof(int64_t), mem_id, node, port, timeout);
}
float rparam_get_float(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_FLOAT, sizeof(float), mem_id, node, port, timeout);
}
double rparam_get_double(void * out, uint16_t addr, int mem_id, int node, int port, int timeout) {
	return rparam_get_single(out, addr, PARAM_DOUBLE, sizeof(double), mem_id, node, port, timeout);
}
