/**
 * Echo server (RFC 862)
 */

/* stdlib */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>

/* system */
#include <pwd.h>
#include <grp.h>

/* networking */
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MSGSIZE 2048

#define MAX(x,y) ((x) > (y) ? (x) : (y))

int VERBOSE = 0;
int PORT = 7;
int QUEUE = 64;
char USER[32];
char ADDR[INET_ADDRSTRLEN] = "0.0.0.0";
char LOGFILE[80];
char *HELP =
"NAME\n"
"   echod - simple echo server\n\n"
"SYNOPSIS\n"
"    echod -a [ADDRESS] -p [PORT] -u [USERNAME] -q [QUEUE SIZE] -l [LOG FILE] -v\n"
"    echod -h\n\n"
"DESCRIPTION\n"
"    Send back to the originating source any data it receives\n\n"
"    -a [ADDRESS]\n"
"        Address to bind. Defaults to 0.0.0.0 Example: -a 0.0.0.0\n\n"
"    -p [PORT]\n"
"        Port to use. Defaults to 7. Example: -p 4499\n\n"
"    -u [USERNAME]\n"
"        User to switch after binding port. Example -u nobody\n\n"
"    -q [QUEUE SIZE]\n"
"        Size of the connection queue waiting for response. Defaults to 64.\n"
"        Example: -q 128\n\n"
"    -l [LOG FILE]\n"
"        Log file for errors. Defaults to stdout. Example -l echod.log\n\n"
"    -v\n"
"        Verbose mode. Defaults to false. Example: -v\n\n"
"    -h\n"
"        Show this message.\n"
"";

static inline void
print(char *fmt, ...)
{
	va_list ap;
	time_t now;
	char buf[24];

	now = time(NULL);
	strftime(buf, sizeof(buf), "%F %H:%M:%S ", localtime(&now));
	fputs(buf, stdout);

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);

	if (VERBOSE && errno)
		fputs(strerror(errno), stdout), errno = 0;

	fputc('\n', stdout);
	fflush(stdout);
}

static inline void
die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	print(fmt, ap);
	va_end(ap);

	exit(-1);
}

static void
parse_args(int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "p:q:a:u:l:vh")) > 0)
	{
		switch (opt)
		{
			case 'v':
				VERBOSE = 1;
			break;
			case 'p':
				PORT = atoi(optarg);
			break;
			case 'q':
				QUEUE = atoi(optarg);
			break;
			case 'a':
				strncpy(ADDR, optarg, sizeof(ADDR));
			break;
			case 'u':
				strncpy(USER, optarg, sizeof(USER));
			break;
			case 'l':
				strncpy(LOGFILE, optarg, sizeof(LOGFILE));
			break;
			case 'h':
				puts(HELP); exit(0);
			break;
			default:
				die("Could not parse all arguments");
		}
	}
}

static void
change_user(const char *name)
{
	struct passwd *pw;

	if (name == NULL || name[0] == '\0')
		return;

	if ((pw = getpwnam(name)) == NULL)
		die("Could not find user '%s'", name);

	/* no point in switching to the current user */
	if (pw->pw_uid == getuid())
		return;

	if (initgroups(pw->pw_name, pw->pw_gid) 
		|| setgid(pw->pw_gid)
		|| setuid(pw->pw_uid))
		die("Could not switch to user %s", name);
}

static int
listen_on_port(int port)
{
	struct sockaddr_in addr;
	int sock;
	int y = 1;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Failed to create socket");

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) < 0)
	{
		close(sock);
		die("Failed to set reusable socket");
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		close(sock);
		die("Failed to bind to port");
	}

	if (listen(sock, QUEUE) < 0)
	{
		close(sock);
		die("Failed to listen on port");
	}

	print("Listening on %s:%d as %s", ADDR, PORT, USER);

	return sock;
}

static void
serve_client(int sock, fd_set *fd)
{
	int got, sent = 0;
	char msg[MSGSIZE];

	if ((got = recv(sock, &msg, sizeof(msg)-1, 0)) <= 0)
	{
		if (got < 0)
			print("Could not receive data");
		else	
			print("Client from socket %d disconnected", sock);

		close(sock);
		FD_CLR(sock, fd);
		return;
	}

	//msg[got] = '\0';

	/* send back everything that was received */
	while ((sent = send(sock, msg + sent, got, 0)) > 0)
		got -= sent;
}

static int
accept_conn(int sv, fd_set *fd)
{
	struct sockaddr_in info;
	socklen_t len = sizeof(info);
	int sock;

	if ((sock = accept(sv, (struct sockaddr *)&info, &len)) < 0)
	{
		print("accept");
		return -1;
	}

	/* add client to the set */
	FD_SET(sock, fd);

	print("New connection from %s on socket %d", 
		inet_ntoa(info.sin_addr), sock);

	return sock;
}

int
main(int argc, char **argv)
{
	fd_set master, copy;
	struct timeval to;
	int sv, cl, top, i;
	FILE *log;

	parse_args(argc, argv);

	if (USER[0] == '\0')
		getlogin_r(USER, sizeof(USER));

	if (LOGFILE[0] != '\0')
	{
		if ((log = fopen(LOGFILE, "a")) == NULL)
			die("Failed to open log file");

		if (dup2(fileno(log), fileno(stdout)) < 0)
			die("Failed to redirect stdout");

		fclose(log);
	}

	sv = listen_on_port(PORT);
	change_user(USER);

	/* init set */
	FD_ZERO(&master);
	FD_ZERO(&copy);
	FD_SET(sv, &master);
	top = sv;

	for (;;)
	{
		/* some implementations of select() modify
		 * the timeout parameter so we are considering 
		 * it to be undefined after each call */
		to.tv_sec = 0;
		to.tv_usec = 0;

		/* we have to copy this because select will modify it */
		copy = master;

		/* handle select errors */
		if (select(top+1, &copy, NULL, NULL, &to) < 0)
		{
			if (errno == EINTR)
				continue;
			else
				die("Failed to select() connections");
		}

		for (i = 0; i <= top; i++)
		{
			if (!FD_ISSET(i, &copy))
				continue;

			if (i != sv)
			{
				/* data from client */
				serve_client(i, &master);
			}
			else
			{
				/* new connection */
				cl = accept_conn(sv, &master);
				top = MAX(cl, top);
			}
		}
	}
}
