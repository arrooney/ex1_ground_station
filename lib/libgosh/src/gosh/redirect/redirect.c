/**
 * @file redirect.c
 *
 * This implements redirection of stdio over a local socket.
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2015 GomSpace ApS. All rights reserved.
 */

#include <gosh/redirect.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

#include <util/console.h>
#include <log/log.h>

#define READ_END	0
#define WRITE_END	1

static pthread_t gosh_redirect_handle;
static unsigned short redirect_port;

static void *gosh_redirect_thread(void *params)
{
	int sockfd, connectionfd = -1;
	socklen_t clilen;
	char buffer[1];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i; 
	int pipe_stdout[2], pipe_stdin[2];
	bool disconnect = false;

	struct pollfd fds[3];
	int nfds = 1;

	/* Create pipe sets */
	if (pipe(pipe_stdout) < 0 || pipe(pipe_stdin) < 0) {
		log_error("failed to create pipes");
		return NULL;
	}

	/* Set pipes nonblocking - stdin reads should still be blocking */
	fcntl(pipe_stdout[READ_END], F_SETFL, O_NONBLOCK);
	fcntl(pipe_stdout[WRITE_END], F_SETFL, O_NONBLOCK);
	fcntl(pipe_stdin[WRITE_END], F_SETFL, O_NONBLOCK);

	/* Create accept socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("failed to create socket");
		return NULL;
	}

	/* Enable port reuse */
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	/* Set socket nonblocking */
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	/* Initialize socket structure */
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(redirect_port);

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		log_error("failed to bind socket");
		return NULL;
	}

	/* Redirect stdio, see you on the other side */
	log_info("redirecting stdio to port %hu", redirect_port);
	dup2(pipe_stdout[WRITE_END], STDOUT_FILENO);
	dup2(pipe_stdin[READ_END], STDIN_FILENO);

	/* Start listening for connections */
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	/* Prepare poll events */
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;
	nfds = 1;

	while (1) {
		/* Wait for event on fds */
		poll(fds, nfds, -1);

		for (i = 0; i < nfds; i++) {
			if (fds[i].revents != POLLIN)
				continue;

			if (fds[i].fd == sockfd) {
				fprintf(stderr, "Connect!\n");
				connectionfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
				if (connectionfd < 0) {
					disconnect = true;
					break;
				}

				/* Set socket nonblocking */
				fcntl(connectionfd, F_SETFL, O_NONBLOCK);

				/* Add connection to poll list */
				fds[1].fd = pipe_stdout[READ_END];
				fds[1].events = POLLIN;
				fds[2].fd = connectionfd;
				fds[2].events = POLLIN;
				nfds = 3;

				/* Refresh gosh */
				console_update();

				break;
			} else if (fds[i].fd == connectionfd) {
				n = read(connectionfd, buffer, sizeof(buffer));
				if (n < 1) {
					disconnect = true;
					break;
				}
				n = write(pipe_stdin[WRITE_END], buffer, n);
				if (n < 1) {
					disconnect = true;
					break;
				}
			} else if (fds[i].fd == pipe_stdout[READ_END]) {
				n = read(pipe_stdout[READ_END], buffer, sizeof(buffer));
				if (n < 1) {
					disconnect = true;
					break;
				}
				n = write(connectionfd, buffer, n);
				if (n < 1) {
					disconnect = true;
					break;
				}
			}
		}

		/* Remove sockets from poll on error */
		if (disconnect) {
			fprintf(stderr, "Disconnect\n");
			close(connectionfd);
			nfds = 1;
			disconnect = false;
		}
	}

	return NULL;
}

int gosh_redirect(unsigned short port)
{
	int ret;

	redirect_port = port;

	ret = pthread_create(&gosh_redirect_handle, NULL, gosh_redirect_thread, NULL);
	if (ret != 0) {
		log_error("Failed to redirect stdio to %hu\n", redirect_port);
		return ret;
	}

	return 0;
}
