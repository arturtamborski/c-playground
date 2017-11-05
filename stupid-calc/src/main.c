#include <stdio.h>

enum
{
	CALC_STACK_MAX = 1000,
};

struct calc_context
{
	unsigned int sp;
	int stack[CALC_STACK_MAX];
} calc;

void
push(int x)
{
	if (calc.sp < CALC_STACK_MAX)
		calc.stack[calc.sp++] = x;
	else
		printf("error: stack is full, can\'t push %d\n", x);
}

int
pop(void)
{
	if (calc.sp > 0)
		return calc.stack[--calc.sp];
	else
		printf("error: stack is empty\n");

	return 0;
}

void
print(void)
{
	int val = pop();
        printf("\t%d\n", val);
	push(val);
}

int
main(int argc, char **argv)
{
	int op;
	int reg;

	while ((op = getchar_unlocked()) != EOF)
	{
		switch (op)
		{
			case '0': case '1': case '2': case '3': case '4': 
			case '5': case '6': case '7': case '8': case '9':
				push(op);
				puts("");
				break;

			case '+':
				push(pop() + pop());
				break;

			case '-':
				reg = pop();
				push(pop() - reg);
				break;

			case '*':
				push(pop() * pop());
				break;

			case '/':
				reg = pop();
				if (reg != 0)
					push(pop() / reg);
				else
					puts("error: can\'t divide by zero!\n");
				break;

			case '\n':
				printf("\t%d\n", pop());
				break;

			case ' ': case '\t':
			default:
				printf("error: unknown operator %c\n", op);
				break;
		}
	}
}
