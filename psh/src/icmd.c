#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>

#include "icmd.h"


#define ICMDS_SIZE                             \
        (sizeof(g_icmds) / sizeof(g_icmds[0])) \


#define ICMD(name)                             \
        {                                      \
                .cmd = #name,                  \
                .len = sizeof(#name) - 1,      \
                .func = icmd_##name,           \
        }                                      \


struct icmd
{
        char *cmd;
        int len;
        int (*func)(int, char **);

} g_icmds[] =
{
        ICMD(help),
        ICMD(echo),
        ICMD(exit),
        ICMD(cls),
        ICMD(cd),
        ICMD(ls),
};



#define icmd_die(status, ...)                            \
        {                                                \
                _icmd_die(status, argv[0], __VA_ARGS__); \
                return status;                           \
        }                                                \


static void
_icmd_die(int status, char *func, char *format, ...)
{
        va_list args;

        va_start(args, format);
        if (status != 0)
                printf("%s: Error: ", func);

        vprintf(format, args);
        if (status != 0)
                putchar('!');

        putchar('\n');
        if (status < 0)
                perror("Reason");

        va_end(args);
}


int
find_icmd(char *cmd)
{
	size_t i;

	for (i = 0; i < ICMDS_SIZE; i++)
		/* I'm doing a len+1 here because I want to compare with NUL explicitly */
		if (strncmp(g_icmds[i].cmd, cmd, g_icmds[i].len+1) == 0)
			return i;

	return -1;
}


int
call_icmd(int idx, int argc, char **argv)
{
	return g_icmds[idx].func(argc, argv);
}


int
icmd_help(int argc, char **argv)
{
        size_t i;

        puts(
                "\n  psh  -  poor man's shell"
                "\n          written by:     "
                "\n          Artur Tamborski "
                "\n                          "
                "\n  internal commands:      "
        );

        for (i = 0; i < ICMDS_SIZE; i++)
                printf("    - %s\n", g_icmds[i].cmd);

        putchar('\n');

        return 0;
}


int
icmd_echo(int argc, char **argv)
{
        int i;

        for (i = 1; i < argc; i++)
                printf("%s%c", argv[i], (argc - i != 1) ? ' ' : '\n');

        return 0;
}


int
icmd_exit(int argc, char **argv)
{
        if (argc == 1)
                exit(0);

        exit(strtol(argv[1], NULL, 10));

        return 0;
}


int
icmd_cls(int argc, char **argv)
{
        int i = 24;

        while (i--)
                putchar('\n');

        return 0;
}


int
icmd_cd(int argc, char **argv)
{
        char def_path[] = ".";
        char *path = def_path;

        if (argc > 2)
                icmd_die(1, "too many arguments given");

        if (argc == 2)
                path = argv[1];

        if (chdir(path))
                icmd_die(-1, "could not change directory");

        return 0;
}


int
icmd_ls(int argc, char **argv)
{
        char def_path[] = "./";
        char *path = def_path;
        struct dirent *d;
        DIR *dir;
        int i = 1;

        if (argc > 1)
                path = argv[1];

        do
        {
                if ((dir = opendir(path)) == NULL)
                        icmd_die(-1, "Could not open directory '%s'", path);

		/* print terminating slash if it wasn't given in argv */
		printf("\n%s%c\n", path, (path[strlen(path)-1] != '/') ? '/' : '\0');

                while ((d = readdir(dir)) != NULL)
		{
			/* skip printing current directory '.\0' and parent directory '..\0' */
			if (d->d_name[0] == '.' && (d->d_name[1] == '\0'
				|| (d->d_name[1] == '.' && d->d_name[2] == '\0')))
				continue;

			printf("  %s%c\n", d->d_name, (d->d_type == DT_DIR) ? '/' : '\0');
		}

		putchar('\n');
                closedir(dir);
                path = argv[++i];

        } while (i < argc);

        return 0;
}
