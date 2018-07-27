/**
 * ROT13 server
 * based on libevent-book
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

/* libevent */
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

/* constants */
#define MAX_LINE (1024 * 16)

/* globals */
int VERBOSE = 0;
int PORT = 2244;
int QUEUE = 64;
char USER[32];
char ADDR[INET_ADDRSTRLEN] = "127.0.0.1";
char LOGFILE[80];
char *HELP =
"NAME\n"
"   rot13d - simple rot13 server\n\n"
"SYNOPSIS\n"
"    rot13d -a [ADDRESS] -p [PORT] -u [USERNAME] -q [QUEUE SIZE] -l [LOG FILE] -v\n"
"    rot13d -h\n\n"
"DESCRIPTION\n"
"    Send back to the originating source rot13 data \n\n"
"    -a [ADDRESS]\n"
"        Address to bind. Defaults to 127.0.0.1 Example: -a 0.0.0.0\n\n"
"    -p [PORT]\n"
"        Port to use. Defaults to 2244. Example: -p 4499\n\n"
"    -u [USERNAME]\n"
"        User to switch after binding port. Example -u nobody\n\n"
"    -q [QUEUE SIZE]\n"
"        Size of the connection queue waiting for response. Defaults to 64.\n"
"        Example: -q 128\n\n"
"    -l [LOG FILE]\n"
"        Log file for errors. Defaults to stdout. Example -l error.log\n\n"
"    -v\n"
"        Verbose mode. Defaults to false. Example: -v\n\n"
"    -h\n"
"        Show this message.\n"
"";

/* utility functions */
static void print(char *fmt, ...)
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

static void die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	print(fmt, ap);
	va_end(ap);

	exit(-1);
}

static void parse_args(int argc, char **argv)
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

static void change_user(const char *name)
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

static int listen_on_port(int port)
{
	struct sockaddr_in sin;
	int sock;
	int y = 1;

	memset(&sin, 0, sizeof(sin));
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(ADDR);
	sin.sin_family = AF_INET;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Failed to create socket");

	evutil_make_socket_nonblocking(sock);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) < 0)
	{
		close(sock);
		die("Failed to set reusable socket");
	}

	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		close(sock);
		die("Failed to bind to port");
	}

	if (listen(sock, QUEUE) < 0)
	{
		close(sock);
		die("Failed to listen on port");
	}

	print("Listening on %s:%d as %s\n", ADDR, PORT, USER);

	return sock;
}

/* program functions */
inline char rot13_c(char c)
{
	if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
		return c + 13;

	if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
		return c - 13;

	return c;
}

/* network functions */
void read_cb(struct bufferevent *bev, void *ctx)
{
	(void)(ctx);

	char buf[1024];
	struct evbuffer *in, *out;
	char *line;
	size_t i, n;

	in = bufferevent_get_input(bev);
	out = bufferevent_get_output(bev);

	while ((line = evbuffer_readln(in, &n, EVBUFFER_EOL_LF)))
	{
		/* encode received line */
		for (i = 0; i < n; i++)
			line[i] = rot13_c(line[i]);

		evbuffer_add(out, line, n);
		evbuffer_add(out, "\n", 1);
		free(line);
	}

	/* input is too long, we can't handle it above MAX_LINE
	 * so we're just processing what there is and go on */
	if (evbuffer_get_length(in) >= MAX_LINE)
	{
		while (evbuffer_get_length(in))
		{
			n = evbuffer_remove(in, buf, sizeof(buf));

			for (i = 0; i < n; i++)
				buf[i] = rot13_c(buf[i]);

			evbuffer_add(out, buf, n);
		}

		evbuffer_add(out, "\n", 1);
	}
}

void error_cb(struct bufferevent *bev, short error, void *ctx)
{
	(void)(ctx);

	if (error & BEV_EVENT_EOF)
	{
		print("conneciton closed");

	} else if (error & BEV_EVENT_ERROR)
	{
		bufferevent_free(bev);
		die("event error");

	} else if (error & BEV_EVENT_TIMEOUT)
	{
		print("timeout error");
	}

	bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short events, void *arg)
{
	(void)(events);

	struct event_base *base = arg;
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	struct bufferevent *bev;
	int fd;
	
	if ((fd = accept(listener, (struct sockaddr *)&sin, &len)) < 0)
		die("cant accept connection");

	if (fd > FD_SETSIZE)
	{
		close(fd);
		return;
	}

	evutil_make_socket_nonblocking(fd);
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, read_cb, NULL, error_cb, NULL);
	bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	print("new connection from %s", inet_ntoa(sin.sin_addr));
}

int main(int argc, char **argv)
{
	struct event *listener_event;
	evutil_socket_t listener;
	struct event_base *base;
	FILE *log;
	
	setvbuf(stdout, NULL, _IONBF, 0);
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

	if (!(base = event_base_new()))
		return -1;

	/* bind to port and drop privileges */
	listener = listen_on_port(PORT);
	change_user(USER);

	listener_event = event_new(base, listener,
		EV_READ | EV_PERSIST, do_accept, base);

	event_add(listener_event, NULL);
	event_base_dispatch(base);

	return 0;
}
