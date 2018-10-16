/* ==================================== *
 * Brainfunk -- A Brainf**k Interpreter *
 * Neo_Chen			        *
 * ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <libbrainfunk.h>

/* init */

memory_t *memory;
unsigned int ptr=0;
stack_type *stack;
unsigned int stack_ptr=0;
code_t *code;
unsigned int code_ptr=0;

int debug=0;
int color=1;

unsigned int memsize=DEF_MEMSIZE;
unsigned int codesize=DEF_CODESIZE;
unsigned int stacksize=DEF_STACKSIZE;

void debug_output(void)
{
	printw("code=%u:%c\n", code_ptr, code[code_ptr]);
	printw("stack=%u:0x%0x\n", stack_ptr, stack[stack_ptr]);
	printw("ptr=%0x:0x%0x\n", ptr, memory[ptr]);
	printw("--------\n");
}

void output(memory_t c)
{
	printw("%c", c);
}

memory_t input(void)
{
	return (memory_t)getch();
}

int main(int argc, char **argv)
{
	/* Init */
	memory	= calloc(MEMSIZE, sizeof(memory_t));
	code	= calloc(CODESIZE, sizeof(code_t));
	stack	= calloc(STACKSIZE, sizeof(stack_type));

	/* Parse Argument */
	FILE *corefile;
	int opt;

	if(!(argc >= 2))
	{
		puts("argc < 2!");
	}
	else
	{
		while((opt = getopt(argc, argv, "hdf:c:s:")) != -1)
		{
			switch(opt)
			{
				case 's': /* Read size from argument */
#ifndef FAST
					sscanf(optarg, "%u,%u,%u", &memsize, &codesize, &stacksize);
					if(memsize == 0 || codesize == 0 || stacksize == 0)
						panic("?SIZE=0");
					free(memory);
					free(code);
					free(stack);
					memory	= calloc(memsize, sizeof(memory_t));
					code	= calloc(codesize, sizeof(code_t));
					stack	= calloc(stacksize, sizeof(stack_type));
#endif
					break;
				case 'f': /* File */
					if(strcmp(optarg, "-"))
					{
						if((corefile = fopen(optarg, "r")) == NULL)
						{
							perror(optarg);
							exit(8);
						}
						read_code(corefile);
						fclose(corefile);
					}
					else
						read_code(stdin);
					break;
				case 'c': /* Code */
					strncpy(code, optarg, CODESIZE);
					break;
				case 'h': /* Help */
					printf("Usage: %s [-h] [-f file] [-c code] [-s memsize,codesize,stacksize] [-d]\n", argv[0]);
					break;
				case 'k':
					color = 1;
				case 'd': /* Debug */
					puts("Enabled Debug verbose message");
					debug = TRUE;
					break;
				default:
					exit(1);

			}
		}
	}

	initscr();
	printw("memory	= %p[%d]\n", memory, MEMSIZE);
	printw("code	= %p[%d]\n", code, CODESIZE);
	printw("stack	= %p[%d]\n\n", stack, STACKSIZE);
	refresh();

	while(code[code_ptr] != '\0')
	{
		if(debug)
			debug_output();
		interprete(code[code_ptr]);
		refresh();
	}

	endwin();
	free(memory);
	free(code);
	free(stack);
	return 0;
}