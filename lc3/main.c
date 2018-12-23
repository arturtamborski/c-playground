#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>


#define GET_R0 ((g_instr >> 9) & 0x7)
#define GET_R1 ((g_instr >> 6) & 0x7)
#define GET_R2 ((g_instr >> 5) & 0x1)

#define GET_R0_SE (sign_extend(g_instr & 0x7FF, 11))
#define GET_R1_SE (sign_extend(g_instr & 0x1FF, 9))
#define GET_R2_SE (sign_extend(g_instr & 0x3F, 6))




enum
{
	PC_START = 0x3000,
	MEM_SIZE = UINT16_MAX,

	FLAG_POS = 1 << 0, /* positive */
	FLAG_ZRO = 1 << 1, /* zero */
	FLAG_NEG = 1 << 2, /* negative */
	
	MR_KBSR = 0xFE00, /* kbd status */
	MR_KBDR = 0xFE02, /* kbd data */

};


enum
{
	REG_R0 = 0,
	REG_R1,
	REG_R2,
	REG_R3,
	REG_R4,
	REG_R5,
	REG_R6,
	REG_R7,
	REG_PC,
	REG_IF,
	REG_COUNT,
};


enum
{
	OP_BR = 0, /* branch */
	OP_ADD,    /* add  */
	OP_LD,     /* load */
	OP_ST,     /* store */
	OP_JSR,    /* jump register */
	OP_AND,    /* bitwise and */
	OP_LDR,    /* load register */
	OP_STR,    /* store register */
	OP_RTI,    /* unused */
	OP_NOT,    /* bitwise not */
	OP_LDI,    /* load indirect */
	OP_STI,    /* store indirect */
	OP_JMP,    /* jump */
	OP_RES,    /* reserved (unused) */
	OP_LEA,    /* load effective address */
	OP_TRAP,   /* execute trap */
};


enum
{
	TRAP_START = 0x20,
	TRAP_GETC = 0x20, /* get character from keyboard */
	TRAP_OUT,         /* output a character */
	TRAP_PUTS,        /* output a word string */
	TRAP_IN,          /* input a string */
	TRAP_PUTSP,       /* output a byte string */
	TRAP_HALT,        /* halt the program */
};




uint16_t g_instr;
uint16_t g_opcode;
uint16_t g_running;

uint16_t g_mem[MEM_SIZE];
uint16_t g_regs[REG_COUNT];

struct termios g_tio;




void disable_input_buffering(void)
{
	struct termios tio;

	tcgetattr(STDIN_FILENO, &g_tio);
	tio = g_tio;
	tio.c_lflag &= ~ICANON & ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &tio);
}


void restore_input_buffering(void)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &g_tio);
}


void handle_interrupt(int signal)
{
	(void)signal;

	restore_input_buffering();
	printf("\n");
	exit(-2);
}


uint16_t sign_extend(uint16_t x, int bit_count)
{
	if ((x >> (bit_count - 1)) & 1)
		x |= 0xFFFF << bit_count;

	return x;
}


uint16_t swap16(uint16_t x)
{
	return (x << 8) | (x >> 8);
}


void update_flags(uint16_t r)
{
	uint16_t flag = FLAG_POS;

	if (g_regs[r] == 0)
		flag = FLAG_ZRO;

	if (g_regs[r] >> 15)
		flag = FLAG_NEG;

	g_regs[REG_IF] = flag;
}


int read_image(char *path)
{
	uint16_t *p, max, origin;
	size_t read;
	FILE *f;
	
	if((f = fopen(path, "rb")) == NULL)
		return 1;

	read = fread(&origin, sizeof(origin), 1, f);
	origin = swap16(origin);

	max = UINT16_MAX - origin;
	p = g_mem + origin;

	read = fread(p, sizeof(uint16_t), max, f);

	while (read-- > 0)
	{
		*p = swap16(*p);
		++p;
	}

	fclose(f);

	return 0;
}


uint16_t check_key(void)
{
	struct timeval timeout = {0};
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	return select(1, &readfds, NULL, NULL, &timeout) != 0;
}


void mem_write(uint16_t addr, uint16_t val)
{
	g_mem[addr] = val;
}


uint16_t mem_read(uint16_t addr)
{
	if (addr == MR_KBSR)
	{
		if (check_key())
		{
			g_mem[MR_KBSR] = (1 << 15);
			g_mem[MR_KBDR] = getchar();
		}
		else
		{
			g_mem[MR_KBSR] = 0;
		}
	}

	return g_mem[addr];
}




void trap_func_getc(void)
{
	g_regs[REG_R0] = (uint16_t)getchar();
}


void trap_func_out(void)
{
	putchar((char)g_regs[REG_R0]);
}


void trap_func_puts(void)
{
	uint16_t *c;

	c = g_mem + g_regs[REG_R0];

	while (*c)
		putc((char)*c++, stdout);
}


void trap_func_in(void)
{
	printf("Enter character: ");

	g_regs[REG_R0] = (uint16_t)getchar();
}


void trap_func_putsp(void)
{
	uint16_t tmp, *c;

	c = g_mem + g_regs[REG_R0];

	while (*c)
	{
		tmp = (*c) >> 8;
		putchar((*c++) & 0xFF);

		if (tmp)
			putchar(tmp);
	}
}


void trap_func_halt(void)
{
	puts("Halt!");
	g_running = 0;
}


typedef void (trap_func_t)(void);

trap_func_t *g_traps[] =
{
	trap_func_getc,
	trap_func_out,
	trap_func_puts,
	trap_func_in,
	trap_func_putsp,
	trap_func_halt,
};




void op_func_br(void)
{
	uint16_t off, flag;

	flag = GET_R0;
	off = GET_R1_SE;

	if (flag & g_regs[REG_IF])
		g_regs[REG_PC] += off;
}


void op_func_add(void)
{
	uint16_t r0, r1, flag;

	r0 = GET_R0;
	r1 = GET_R1;
	flag = GET_R2;

	if (flag)
		g_regs[r0] = g_regs[r1] + sign_extend(g_instr & 0x1F, 5);
	else
		g_regs[r0] = g_regs[r1] + g_regs[g_instr & 0x7];

	update_flags(r0);
}


void op_func_ld(void)
{
	uint16_t r0, off;

	r0 = GET_R0;
	off = GET_R1_SE;

	g_regs[r0] = mem_read(g_regs[REG_PC] + off);

	update_flags(r0);
}


void op_func_st(void)
{
	uint16_t r0, off;

	r0 = GET_R0;
	off = GET_R1_SE;

	mem_write(g_regs[REG_PC] + off, g_regs[r0]);
}


void op_func_jsr(void)
{
	uint16_t r1, off, flag;

	off = GET_R0_SE;
	r1 = GET_R1;
	flag = (g_instr >> 11) & 1;

	g_regs[REG_R7] = g_regs[REG_PC];

	if (flag)
		g_regs[REG_PC] += off;
	else
		g_regs[REG_PC] = g_regs[r1];
}


void op_func_and(void)
{
	uint16_t r0, r1, flag;

	r0 = GET_R0;
	r1 = GET_R1;
	flag = GET_R2;

	if (flag)
		g_regs[r0] = g_regs[r1] & sign_extend(g_instr & 0x1F, 5);
	else
		g_regs[r0] = g_regs[r1] & g_regs[g_instr & 0x7];

	update_flags(r0);
}


void op_func_ldr(void)
{
	uint16_t r0, r1, off;

	r0 = GET_R0;
	r1 = GET_R1;
	off = GET_R2_SE;

	g_regs[r0] = mem_read(g_regs[r1] + off);
	
	update_flags(r0);
}


void op_func_str(void)
{
	uint16_t r0, r1, off;

	r0 = GET_R0;
	r1 = GET_R1;
	off = GET_R2_SE;

	mem_write(g_regs[r1] + off, g_regs[r0]);
}


void op_func_rti(void)
{
	abort();
}


void op_func_not(void)
{
	uint16_t r0, r1;

	r0 = GET_R0;
	r1 = GET_R1;

	g_regs[r0] = ~g_regs[r1];

	update_flags(r0);
}


void op_func_ldi(void)
{
	uint16_t r0, off;

	r0 = GET_R0;
	off = GET_R1_SE;

	g_regs[r0] = mem_read(mem_read(g_regs[REG_PC] + off));

	update_flags(r0);
}


void op_func_sti(void)
{
	uint16_t r0, off;

	r0 = GET_R0;
	off = GET_R1_SE;

	mem_write(mem_read(g_regs[REG_PC] + off), g_regs[r0]);
}


void op_func_jmp(void)
{
	uint16_t r1;

	r1 = GET_R1;

	g_regs[REG_PC] = g_regs[r1];
}


void op_func_res(void)
{
	abort();
}


void op_func_lea(void)
{
	uint16_t r0, off;

	r0 = GET_R0;
	off = GET_R1_SE;

	g_regs[r0] = g_regs[REG_PC] + off;

	update_flags(r0);
}


void op_func_trap(void)
{
	int idx;

	/* subtract offset, so we get index starting at 0 */
	idx = (g_instr & 0xFF) - TRAP_START;
	g_traps[idx]();
	fflush(stdout);
}


typedef void (op_func_t)(void);

op_func_t *g_ops[] =
{
	op_func_br,
	op_func_add,
	op_func_ld,
	op_func_st,
	op_func_jsr,
	op_func_and,
	op_func_ldr,
	op_func_str,
	op_func_rti,
	op_func_not,
	op_func_ldi,
	op_func_sti,
	op_func_jmp,
	op_func_res,
	op_func_lea,
	op_func_trap,
};




int main(int argc, char **argv)
{
	int i;
	
	if (argc < 2)
		return puts("lc3 [<image file> ...] "), 1;

	for (i = 0; i < argc; i++)
		if (read_image(argv[i]))
			return printf("failed to load image '%s'\n", argv[i]), 1;

	disable_input_buffering();
	signal(SIGINT, handle_interrupt);

	/* set start position */
	g_regs[REG_PC] = PC_START;
	g_running = 1;

	while (g_running)
	{
		/* fetch next instruction */
		g_instr = mem_read(g_regs[REG_PC]++);
		g_opcode = g_instr >> 12;

		/* find and execute given opcode */
		g_ops[g_opcode]();
	}

	restore_input_buffering();
}
