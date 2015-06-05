/**
 * GomSpace Parameter System
 *
 * @author Johan De Claville Christiansen
 * Copyright 2014 GomSpace ApS. All rights reserved.
 */

#ifndef PARAM_H_
#define PARAM_H_

#include <conf_param.h>

#if ENABLE_PARAM_LOCAL_CLIENT
#include <param_host.h>
#endif

#if ENABLE_PARAM_CSP_CLIENT
#include <rparam_client.h>
#endif

#if ENABLE_PARAM_SERVER
#include <rparam_server.h>
#endif

#include "param_serializer.h"
#include "param_fletcher.h"
#include "param_string.h"
#include "param_types.h"

#endif /* PARAM_H_ */
