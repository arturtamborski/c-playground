#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

int
main (int argc, char **argv)
{
	DIR *dir;
	struct dirent *ent;

	if (argc < 2)
		return (-1);

	if ((dir = opendir(argv[1])))
	{
		while ((ent = readdir(dir)) != NULL)
		{
			// i should use printf and print
			// some more info about those files
			puts(ent->d_name);
		}

		closedir (dir);
	}

	return (0);
}
