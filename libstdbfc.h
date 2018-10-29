#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MEMSIZE	262144
typedef char memory_t;
typedef unsigned int arg_t;
memory_t *memory;
unsigned int ptr=0;

void add(arg_t arg)
{
	memory[ptr]+=arg;
}

void sub(arg_t arg)
{
	memory[ptr]-=arg;
}

void fwd(arg_t arg)
{
	ptr += arg;
}

void rew(arg_t arg)
{
	ptr -= arg;
}

memory_t in()
{
	return (memory_t)getchar();
}

void out(memory_t arg)
{
	putchar(arg);
	fflush(NULL);
}

void hlt(arg_t arg)
{
	free(memory);
	exit(arg);
}
