#include <stdio.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int
main(int argc, char **argv)
{
	struct hostent *host;
	struct in_addr host_addr;

	if (argc < 2)
		return puts("USAGE: nslookup <domain name>") > 0;

	if ((host = gethostbyname(argv[1])) == NULL)
		return puts("nslookup failed");

	host_addr.s_addr = *((unsigned long *)host->h_addr_list[0]);

	puts(inet_ntoa(host_addr));

	return (0);
}
