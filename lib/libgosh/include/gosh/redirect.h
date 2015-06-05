/**
 * @file redirect.h
 *
 * This implements redirection of stdio over a local socket.
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2015 GomSpace ApS. All rights reserved.
 */

#ifndef _GOSH_REDIREC_H_
#define _GOSH_REDIREC_H_

#define GOSH_DEFAULT_REDIRECT_PORT 5001

int gosh_redirect(unsigned short port);

#endif /* _GOSH_REDIRECT_H_ */
