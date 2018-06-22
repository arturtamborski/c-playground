#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "shell.h"
#include "debug.h"

internal int
_shell_read_line(str *s, size_t len)
{
	int quote = 0;
	size_t i;
	int c;

	for (i = 0; i < len; i++)
	{
		c = getchar_unlocked();

		if (c == '\n' || c == EOF)
			break;

		if (c == '\'' || c == '\"')
			quote ^= 1;

		if (isspace(c) && !quote)
			continue;

		*s++ = (char)c;
	}


	if (i == len)
		*s--;

	*s = '\0';

	return (-(i == len));
}

internal str *
_shell_find_next_token(str *s)
{
	char *c = s;
	char *ret;

	if (!c)
		return (null);

	while (*c) {
		putchar(*c++);
	}

	while (!(isdigit(*c) || isalpha(*c)))
		c++;

	DEBUG_PRINT("test");

	if (!c)
		return (null);

	ret = c;

	while (c && !isspace(*c))
		c++;

	DEBUG_PRINT("test");

	c = '\0';

	return (ret);
}


struct shell *
shell_create(void)
{
	struct shell *shell;

	if ((shell = malloc(sizeof(*shell))) == null)
		return (null);

	memset(shell, 0, sizeof(*shell));

	return (shell);
}

void
shell_destroy(struct shell *shell)
{
	hashmap_destroy(shell->variables);
	hashmap_destroy(shell->functions);
	hashmap_destroy(shell->commands);
	free(shell);
}

int
shell_init(struct shell *shell, size_t ncommands, size_t nfunctions, size_t nvariables)
{
	shell->commands = hashmap_create();
	if (shell->commands == null)
		goto err;

	shell->functions = hashmap_create();
	if (shell->functions == null)
		goto err;

	shell->variables = hashmap_create();
	if (shell->variables == null)
		goto err;

	if (hashmap_init(shell->commands, ncommands) != HASHMAP_OK)
		goto err;

	if (hashmap_init(shell->functions, nfunctions) != HASHMAP_OK)
		goto err;

	if (hashmap_init(shell->variables, nvariables) != HASHMAP_OK)
		goto err;

	return (0);

err:
	shell_finish(shell);

	return (-1);
}

void
shell_finish(struct shell *shell)
{
	struct shell_variable *v;
	struct shell_function *f;
	struct shell_command *c;

	HASHMAP_ITERATE(shell->variables, v,
		free(v);
	);

	HASHMAP_ITERATE(shell->functions, f,
		free(f);
	);

	HASHMAP_ITERATE(shell->commands, c,
		free(c);
	);

	hashmap_finish(shell->variables);
	hashmap_finish(shell->functions);
	hashmap_finish(shell->commands);
}

int
shell_bind_function(struct shell *shell, void* (*addr)(), const str *name, const str *type)
{
	struct shell_function *f;

	if ((f = malloc(sizeof(*f))) == null)
		return (-1);

	f->name = name;
	f->type = type;
	f->addr = (void *(*)())addr;

	if (hashmap_insert(shell->functions, name, f) != HASHMAP_OK)
		return(-1);

	return (0);
}

void
shell_unbind_function(struct shell *shell, const str *name)
{
	struct shell_function *f;

	if (hashmap_remove(shell->functions, name, (void**)&f) != HASHMAP_OK)
		return;

	if (f)
		free(f);
}

struct shell_function *
shell_find_function(struct shell *shell, const str *name)
{
	struct shell_function *f;

	if (hashmap_get(shell->functions, name, (void**)&f) == HASHMAP_OK)
		return (f);

	return (null);
}

int
shell_evaluate(struct shell *shell, const str *expression)
{
	char *c;

	c = _shell_find_next_token(expression);

	DEBUG_PRINTV(c, "s");

	return (-1);
}

void
shell_spawn(struct shell *shell, const str *prompt)
{
#define INPUTLEN 100
	str input[INPUTLEN];
	struct shell_function *f;
	struct shell_command *c;
	int quit = 0;
	int ret;
	str *token;

	while (!quit)
	{
		memset(input, 0, INPUTLEN);
		fputs(prompt, stdout);
		
		ret = _shell_read_line(input, INPUTLEN);

		token = _shell_find_next_token(input);
		DEBUG_PRINTV(ret, "i");
		DEBUG_PRINTV(input, "s");

		if (strcmp(input, ".quit") == 0)
			break;

		//shell_evaluate(shell, input);

//		if (hashmap_get(shell->commands, input, (void **)&c) == HASHMAP_OK)
//		{
//			c->addr();
//			continue;
//		}
//
//		if (hashmap_get(shell->functions, input, (void **)&f) == HASHMAP_OK)
//		{
//			SHELL_CALL_FUNCTION(shell, f, "test");
//
//			continue;
//		}
	}
#undef INPUTLEN
}
