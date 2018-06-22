#include "types.h"
#include "debug.h"
#include "shell.h"

#include "hashmap.h"

#include "functions.h"

int
main(int argc, const str **argv)
{
	struct shell *shell;

	shell = shell_create();

	if(shell_init(shell, 0, 5, 0))
		DEBUG_PRINT_AND_DIE("cant create shell");

	SHELL_BIND_FUNCTION(shell, fn0, void(void));
	SHELL_BIND_FUNCTION(shell, fn1, void(char *));
	SHELL_BIND_FUNCTION(shell, fn2, void(int, int));
	SHELL_BIND_FUNCTION(shell, fn3, void(int, int, int));
	SHELL_BIND_FUNCTION(shell, fn4, int(int));

	shell_spawn(shell, "> ");

	shell_finish(shell);
	shell_destroy(shell);
}
