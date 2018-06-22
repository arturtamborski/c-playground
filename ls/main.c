#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

int
main (int argc, char **argv)
{
	DIR *dir;
	struct dirent *ent;
	char *path = ".";

	if (argc > 1)
		path = argv[1];

	if (dir = opendir(path))
	{
		while ((ent = readdir(dir)) != NULL)
		{
			// i should use printf and print
			// some more info about these files
			puts(ent->d_name);
		}

		closedir (dir);
	}

	return (0);
}
