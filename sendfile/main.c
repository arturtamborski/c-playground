#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <arpa/inet.h>


#define PORT 2137
#define SIZE 4096


struct __attribute__((packed)) hello
{
	unsigned int magic;
	char user[32];
	char file[256];
};


static void
die(char *s)
{
	perror(s);
	exit(-1);
}


int
run_client(char *host, char *file)
{
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = inet_addr(host);
	server.sin_port        = htons(PORT);
	server.sin_family      = AF_INET;

	int s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (s_server < 0)
		die("Could not create server socket");

	if (connect(s_server, (struct sockaddr *)&server, sizeof(server)) < 0)
		die("Could not connect to server socket");
	
	FILE *fp = stdin;
	if (strcmp(file, "-") != 0)
		fp = fopen(file, "r");

	if (fp == NULL)
		die("Could not open the file or stdin");

	struct hello hello;
	hello.magic = htonl(0xdeadbeef);
	getlogin_r(hello.user, sizeof(hello.user)-1);
	strncpy(hello.file, file, sizeof(hello.file));
	hello.user[sizeof(hello.user)-1] = '\0';
	hello.file[sizeof(hello.file)-1] = '\0';

	ssize_t sent = send(s_server, &hello, sizeof(hello), 0);
	if (sent < 0 || sent != sizeof(hello))
		die("Could not send hello");

	printf("Sending file to %s:%i ", host, PORT);

	char data[SIZE];
	while(!feof(fp))
	{
		size_t got = fread(data, 1, sizeof(data), fp);
		if (got == 0 || (got != sizeof(data) && !feof(fp)))
			die("Could not read from file");

		sent = 0;
		do
			sent = send(s_server, &data[sent], got -= sent, 0);
		while(sent > 0);
	}

	puts("Successfully sent the file");
	close(s_server);
	fclose(fp);
	
	return 0;
}


int
receive_file(char *file, int fd)
{
	FILE *fp = stdout;
	char data[SIZE];
	size_t saved;
	ssize_t got;

	if (strcmp(file, "-") != 0)
	{
		char recv[] = ".recv";
		char name[256];
		strcpy(name, file);
		size_t len = strlen(name);
		size_t diff = sizeof(name) - sizeof(recv);
		len = len > diff ? diff : len;
		memcpy(name + len, recv, sizeof(recv));
		fp = fopen(name, "w");
	}

	if (fp == NULL)
		die("Could not open the file");

	while ((got = recv(fd, data, sizeof(data), 0)))
	{
		if (got < 0)
			die("Could not receive data");

		saved = 0;
		do
			saved = fwrite(&data[saved], 1, got -= saved, fp);
		while(saved > 0);
	}

	fclose(fp);
	return got;
}


int
run_server(char *host)
{
	char *old_host = host;
	if (strcmp(host, "-") == 0)
		host = "127.0.0.1";

	struct sockaddr_in client, server;
	memset(&client, 0, sizeof(client));
	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = inet_addr(host);
	server.sin_port        = htons(PORT);
	server.sin_family      = AF_INET;

	socklen_t addr_size = sizeof(client);

	int s_client, s_server;
	if ((s_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Could not create server socket");

	if (bind(s_server, (struct sockaddr *)&server, sizeof(server)) < 0)
		die("Could not bind server socket to port");

	if (listen(s_server, 32) < 0)
		die("Could not listen on server socket");

	printf("Waiting for connections on %s:%i ...\n", host, PORT);

	for (;;)
	{
		if ((s_client = accept(s_server, (struct sockaddr *)&client, &addr_size)) < 0)
			die("Could not accept connection from the client");

		printf("Connection accepted from %s\n", inet_ntoa(client.sin_addr));

		if (fork() == 0)
		{
			struct hello hello;
			ssize_t got = recv(s_client, &hello, sizeof(hello), 0);
			if (got <= 0)
			{
				if (got < 0)
					perror("Could not receive data");

				if (got == 0)
					puts("Client disconnected");

				if (got != sizeof(hello) || hello.magic != htonl(0xdeadbeef))
					puts("Client sent incorrect hello");

				close(s_client);
				exit(0);
			}

			/* never trust the input ;) */
			hello.user[sizeof(hello.user)-1] = '\0';
			hello.file[sizeof(hello.file)-1] = '\0';

			if (!strlen(hello.user))
				strcpy(hello.user, "[unknown]");

			if (!strlen(hello.file))
				strcpy(hello.file, "-");

			strcpy(hello.file, basename(hello.file));
			printf("Receiving file '%s' from client '%s'\n", hello.file, hello.user);

			if (receive_file(old_host != host ? "-" : hello.file, s_client) < 0)
				puts("Could not receive file");
			else
				puts("Successfully received the file");

			close(s_client);
			exit(0);
		}
	}

	return 0;
}


int
main(int argc, char *argv[])
{
	puts("Run program without any arguments to receive.");
	puts("Run program with single argument to receive at specified address.");
	puts("Run program with two arguments to send file to specified address.");

	if (argc == 1)
		return run_server("127.0.0.1");

	if (argc == 2)
		return run_server(argv[1]);

	if (argc == 3)
		return run_client(argv[2], argv[1]);

	return 0;
}
