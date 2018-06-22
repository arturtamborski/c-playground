/**
 * Quote Of The Day server (RFC 865)
 * Based on https://gkbrk.com/wiki/qotd_protocol/
 */

#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct
{
	const char *citation;
	const char *author;
} QUOTES[] =
{
#include "quotes.inc"
};

size_t QUOTES_SZ = sizeof(QUOTES) / sizeof(QUOTES[0]);

int USE_FORTUNE = 0;
int PORT = 17;
int QUEUE = 64;
char USER[32];
char ADDR[16] = "0.0.0.0";
char *HELP = 
"NAME\n"
"\tqotd - simple quote of the day server\n\n"
"SYNOPSIS\n"
"\tqotd -a [ADDRESS] -p [PORT] -u [USERNAME] -q [QUEUE SIZE] -f\n"
"\tqotd -h\n\n"
"DESCRIPTION\n"
"\tServe random quotes to anyone who connects over TCP\n\n"
"\t-a [ADDRESS]\n"
"\t\tAddress to bind. Defaults to 0.0.0.0 Example: -a 0.0.0.0\n\n"
"\t-p [PORT]\n"
"\t\tPort to use. Defaults to 17. Example: -p 4499\n\n"
"\t-u [USERNAME]\n"
"\t\tUser to switch after binding port. Used only when running as root for\n"
"\t\tBinding to default port 17. Defaults to current user. Example -u nobody\n\n"
"\t-q [QUEUE SIZE]\n"
"\t\tSize of the connection queue waiting for response. Defaults to 64.\n"
"\t\tExample: -q 128\n\n"
"\t-f\n"
"\t\tUse fortune as backend for quotes. Defaults to false. Example: -f\n"
"";

static inline void
die(char *s)
{
	puts(s);
	exit(-1);
}

static void
switch_to_user(const char *username)
{
	struct passwd *pw;

	pw = getpwnam(username);

	if (!pw)
		die("Could not find user");

	if (initgroups(pw->pw_name, pw->pw_gid) 
		|| setgid(pw->pw_gid) || setuid(pw->pw_uid))
		die("Could not change user");
}

static void
parse_args(int argc, char **argv)
{
	int opt = 0;

	while ((opt = getopt(argc, argv, "p:q:a:u:fh")) != -1)
	{
		switch (opt)
		{
			case 'p': PORT = atoi(optarg); break;
			case 'q': QUEUE = atoi(optarg); break;
			case 'a': strncpy(ADDR, optarg, sizeof(ADDR)); break;
			case 'u': strncpy(USER, optarg, sizeof(USER)); break;
			case 'f': USE_FORTUNE = 1; break;
			case 'h': puts(HELP); exit(0); break;
			default:  die("Could not parse all arguments"); break;
		}
	}

	printf("Listening on %s:%d as '%s' with queue size: %i\n", 
		ADDR, PORT, USER, QUEUE);
}

static size_t
get_quote(char *buf, size_t sz)
{
	size_t chars, buf_sz = 0;
	FILE *fp;
	int idx;

	if (USE_FORTUNE)
	{
		if ((fp = popen("fortune", "r")) == NULL)
		{
			puts("Could not open pipe");
			goto failback;
		}

		while ((chars = fread(buf, 1, sz, fp)) > 0)
			buf_sz += chars;

		if (pclose(fp))
			puts("Could not close pipe");

		return buf_sz;
	}

failback:
	idx = rand() % QUOTES_SZ;
	buf_sz = snprintf(buf, sz, "%s\n\n\t-- %s\n",
		QUOTES[idx].citation, QUOTES[idx].author);

	return buf_sz;
}

int
main(int argc, char **argv)
{
	struct sockaddr_in sv, cl;
	int s_sv, s_cl;
	socklen_t s_sz;

	size_t buf_sz;
	char buf[2048];

	getlogin_r(USER, sizeof(USER));
	parse_args(argc, argv);

	srand(time(NULL));

	s_sz = sizeof(sv);
	sv.sin_family = AF_INET;
	sv.sin_port = htons(PORT);
	sv.sin_addr.s_addr = inet_addr(ADDR);

	if ((s_sv = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Could not create socket");
	
	if (bind(s_sv, (struct sockaddr *)&sv, s_sz) < 0)
		die("Could not bind to port");

	if (getuid() == 0)
		switch_to_user(USER);

	if (getuid() == 0)
		puts("Warining! Running with root priviledges");

	listen(s_sv, QUEUE);
	puts("Waiting for connections...");

	for (;;)
	{
		if ((s_cl = accept(s_sv, (struct sockaddr *)&cl, &s_sz)) < 0)
			puts("Could not accept connection");

		printf("Connection accepted from %s\n", inet_ntoa(cl.sin_addr));

		buf_sz = get_quote(buf, sizeof(buf));

		if (send(s_cl, buf, buf_sz, 0) < 0)
			puts("Could not send whole quote");

		close(s_cl);
	}

	return (0);
}
