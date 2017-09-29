#include <stdio.h>
#include <ncurses.h>

enum
{
    CALC_STACK_MAX = 1000,
};

struct calc_context
{
    unsigned int sp;
    float stack[CALC_STACK_MAX];
} calc;

void
push(float x)
{
    if (calc.sp < CALC_STACK_MAX)
        calc.stack[calc.sp++] = x;
    else
        printf("error: stack full, cant push %f\n", x);
}

float
pop(void)
{
    if (calc.sp > 0)
        return calc.stack[--calc.sp];
    else
        printf("error: stack is empty\n");

    return 0.0;
}

int
main(int argc, char **argv)
{
    initscr();
    cbreak();
    //raw();

    int op;
    float reg;

    while ((op = getch()) != EOF)
    {
        switch (op)
        {
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                push((float)reg;
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
                if (reg != 0.0)
                    push(pop() / reg);
                else
                    puts("error: cant divide by zero!\n");
                break;

            case '\n':
                printf("\t%.8f\n", pop());
                break;

            case ' ': case '\t':
            default:
                printf("error: unknown operator %c\n", op);
                break;
        }
    }
}
