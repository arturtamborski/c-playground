#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "icmd.h"


#define INPUT_MAX_LEN 1024
#define PATH_MAX_LEN  4096

#define TOKENS_NUM_MAX 512
#define TOKENS_ARGS_SIZE 2048


char *g_tokens[TOKENS_NUM_MAX];
char g_args[TOKENS_ARGS_SIZE];


void
print(char *format, ...)
{
	va_list args;

	va_start(args, format);
	printf("psh: ");
	vprintf(format, args);
	putchar('\n');
	va_end(args);
}


void
die(int status, char *format, ...)
{
        va_list args;

        va_start(args, format);
        if (status != 0)
                puts("psh: error: ");

        vprintf(format, args);
        if (status != 0)
                puts("! exiting...");

        putchar('\n');
        if (status < 0)
                perror("reason");

        va_end(args);
	exit(status);
}


void
sighup(int signal)
{
        die(1, "Parent process has ben killed");
}


void
sigint(int signal)
{
        die(0, "Caught ^C");
}


int
tokenize(char *input)
{
        char whitespaces[] = " \t\n";
        char **tokens = g_tokens;
        char *args = g_args;
        char *token;
        size_t len;
        int argc = 0;

        token = strtok(input, whitespaces);
        while (token != NULL)
        {
                len = strlen(token) + 1;
                memcpy(args, token, len);
                *tokens++ = args;
                args += len + 1;
                argc++;

                token = strtok(NULL, whitespaces);
        }

        tokens = NULL;
        return argc;
}

int
call_ecmd(int argc, char **argv)
{
	int status;
	char *err;
	pid_t pid;
	
	pid = fork();

	if (pid == -1)
		die(-1, "fork failed");

	/* parent */
	if (pid)
		return wait(&status), status;

	/* child */
	execvp(argv[0], argv);

	/* if we are here then called program failed */
	err = strerror(errno);
	print("%s: %s", argv[0], err);

	return -1;
}


int
main(int argc, char **argv)
{
        char input[INPUT_MAX_LEN];
        char cwd[PATH_MAX_LEN];
        int num_tokens, ret;

        signal(SIGINT, sigint);
        signal(SIGHUP, sighup);
        signal(SIGQUIT, sighup);

        for (;;)
        {
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                        die(1, "Could not get current directory");

                printf("[%s] ", cwd);

                if (!fgets(input, sizeof(input)-1, stdin))
                        die(0, "Caught ^D");

                if ((num_tokens = tokenize(input)) < 0)
                        die(1, "Could not parse input");

		/* no input, start again */
                if (num_tokens == 0)
                        continue;

		/* search for internal commands first */
		if ((ret = find_icmd(g_tokens[0])) >= 0)
		{
			ret = call_icmd(ret, num_tokens, g_tokens);

			if (ret)
				print("Internal command '%s' failed with error code: %d", g_tokens[0], ret);

			continue;
		}
		
		ret = call_ecmd(num_tokens, g_tokens);
		if (ret)
			print("External command '%s' failed with error code: %d", g_tokens[0], ret);
        }
}
