#include <stdio.h>

#define LINE_MAX 1000

int
readline(char *str, int len)
{
    int c, i = 0;

    while(--len > 0 && (c = getchar()) != EOF && c != '\n')
        str[i++] = c;

    if (c == '\n')
        str[i++] = c;

    str[i] = '\0';
    return i;
}

int
strindex(char *str, char *pattern)
{
    int i, j, k;

    for (i = 0; str[i] != '\0'; i++)
    {
        for (j = i, k = 0; pattern[k] != '\0' && str[j] == pattern[k]; j++, k++)
            ;
        if (k > 0 && pattern[k] == '\0')
            return i;
    }
    return -1;
}

int
main(int argc, char **argv)
{
    if (argc < 3)
    {
        puts(
            "./stupid-grep <file> <pattern>\n"
            "\n"
            "   prints lines matching <pattern> in <file>\n"
        );
        return 1;
    }

    char *pattern = argv[2];
    char line[LINE_MAX] = {};

    if (!freopen(argv[1], "r", stdin))
        return 2;

    while (readline(line, LINE_MAX) > 0)
    {
        if (strindex(line, pattern) >= 0)
            puts(line);
    }
}
