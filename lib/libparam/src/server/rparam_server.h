/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef RPARAM_H_
#define RPARAM_H_

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>

void rparam_service_handler(csp_conn_t * conn, csp_packet_t * request_packet);

#endif /* RPARAM_H_ */
