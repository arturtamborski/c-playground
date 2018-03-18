#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include "types.h"
#include "hashmap.h"

struct shell;
struct shell_command;
struct shell_function;
struct shell_variable;

enum shell_type
{
	SHELL_TYPE_COMMAND,
	SHELL_TYPE_FUNCTION,
	SHELL_TYPE_VARIABLE
};

struct shell
{
	struct hashmap	*commands;
	struct hashmap	*functions;
	struct hashmap	*variables;
};

struct shell_command
{
	str	*name;
        void 	*(*addr)();
};

struct shell_function
{
        str	*name;
        str	*type;
        void 	*(*addr)();
};

struct shell_variable
{
	size_t	size;
	str	*name;
	str	*type;
	void	*addr;
};

struct shell 		*shell_create(void);
void			shell_destroy(struct shell *shell);
int			shell_init(struct shell *shell, size_t ncommands, size_t nfunctions, size_t nvariables);
void			shell_finish(struct shell *shell);

int 			shell_bind_function(struct shell *shell, void *(*addr)(), const str *name, const str *type);
void 			shell_unbind_function(struct shell *shell, const str *name);
struct shell_function 	*shell_find_function(struct shell *shell, const str *name);
void			*shell_call_function(struct shell *shell, const str *name, ...);

int			shell_bind_command(struct shell *shell, void *(*addr)(), const str *name);
void			shell_unbind_command(struct shell *shell, const str *name);
struct shell_command	*shell_find_command(struct shell *shell, void *(*addr)(), const str *name);

int			shell_evaluate(struct shell *shell, const str *expression);
void 			shell_spawn(struct shell *shell, const str *prompt);

#define SHELL_BIND_FUNCTION(shell, name, type) \
	shell_bind_function(shell, (void *(*)())name, TOSTR(name), TOSTR(type))

#define SHELL_UNBIND_FUNCTION(shell, name) \
	shell_unbind_function(shell, TOSTR(name))

#define SHELL_CALL_FUNCTION(shell, function, ...) \
		function->addr(shell, __VA_ARGS__)

#endif // SHELL_H_INCLUDED
