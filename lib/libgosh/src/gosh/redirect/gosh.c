#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <poll.h>
#include <netdb.h>
#include <signal.h>

#include <gosh/redirect.h>

static struct termios original;

static void tty_raw(void)
{
	struct termios current;

	if (tcgetattr(STDIN_FILENO, &current) < 0)
		perror("tcgetattr");

	original = current;

	current.c_iflag &= ~(IGNCR | ICRNL);
	current.c_lflag &= ~(ECHO | ICANON | IEXTEN);

	current.c_cc[VMIN] = 1;
	current.c_cc[VTIME] = 0;

	/* Put terminal in raw mode after flushing */
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &current) < 0)
		perror("tcsetattr");
}

static void tty_restore(void)
{
	/* Restore termios settings */
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) < 0)
		perror("tcsetattr");
}

static void exit_handler(void)
{
	tty_restore();
}

static void sigint_handler(int signo)
{
	printf("\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server = gethostbyname("localhost");
	unsigned short portno = GOSH_DEFAULT_REDIRECT_PORT;

	char buffer[256];

	if (argc > 3) {
		fprintf(stderr,"usage %s <hostname> <port>\n", argv[0]);
		exit(0);
	}

	if (argc > 1)
		server = gethostbyname(argv[1]);
	if (argc > 2)
		portno = atoi(argv[2]);

	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		exit(1);
	}

	/* Set socket nonblocking */
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	/* Put TTY in raw mode */
	signal(SIGINT, sigint_handler);
	atexit(exit_handler);
	tty_raw();

	/* Setup poll parameters */
	bool disconnect = false;
	struct pollfd fds[2];
	int nfds = 2;

	fds[0].fd = sockfd;
	fds[0].events = POLLIN;
	fds[1].fd = STDIN_FILENO;
	fds[1].events = POLLIN;

	while (1) {
		poll(fds, nfds, -1);

		int i;
		for (i = 0; i < nfds; i++) {
			if (fds[i].revents != POLLIN)
				continue;

			if (fds[i].fd == STDIN_FILENO) {
				n = read(STDIN_FILENO, buffer, sizeof(buffer));
				if (n < 1) {
					disconnect = true;
					break;
				}
				n = write(sockfd, buffer, n);
				if (n < 1) {
					disconnect = true;
					break;
				}
			} else if (fds[i].fd == sockfd) {
				n = read(sockfd, buffer, sizeof(buffer));
				if (n < 1) {
					disconnect = true;
					break;
				}
				n = write(STDOUT_FILENO, buffer, n);
				if (n < 1) {
					disconnect = true;
					break;
				}
			}
		}

		if (disconnect)
			break;
	}

	close(sockfd);

	return 0;
}
