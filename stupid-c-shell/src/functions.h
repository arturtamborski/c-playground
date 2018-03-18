#include "types.h"
#include "shell.h"

void
fn0(void)
{
	DEBUG_PRINT("hi");
}

void
fn1(struct shell *shell, char *s)
{
	DEBUG_PRINTV(s, "s");
}

void
fn2(int a, int b)
{ 
	DEBUG_PRINTV(a, "i");
	DEBUG_PRINTV(b, "i");
}

void
fn3(int a, int b, str *c)
{ 
	DEBUG_PRINTV(a, "i");
	DEBUG_PRINTV(b, "i");
	DEBUG_PRINTV(c, "s");
}

int
fn4(int x)
{ 
	DEBUG_PRINTV(x, "i");
	return (x * 2);
}
