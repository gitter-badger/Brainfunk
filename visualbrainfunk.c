/* ==================================== *
 * Brainfunk -- A Brainf**k Toolkit     *
 * Neo_Chen			        *
 * ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <ncurses.h>
#include <libbrainfunk.h>

/* init */

memory_t *memory;
arg_t ptr=0;
stack_type *stack;
arg_t stack_ptr=0;
code_t *code;
arg_t code_ptr=0;

int debug=0;
int compat=0;
int delay=1000*100; /* Delay 100ms */
int load_bitcode=0;

size_t memsize=DEF_MEMSIZE;
size_t codesize=DEF_CODESIZE;
size_t stacksize=DEF_STACKSIZE;

bitcode_t *bitcode;
arg_t bitcode_ptr=0;
size_t bitcodesize=DEF_BITCODESIZE;
memory_t *pstack;
arg_t pstack_ptr=0;
size_t pstacksize=DEF_PSTACKSIZE;

#define IO_WINDOW io_win
#define CODE_WINDOW code_win
#define MEM_WINDOW mem_win
#define REG_WINDOW reg_win
#define STACK_WINDOW stack_win

#define MSG_COLOR 1
#define MEM_COLOR 2
#define CODE_COLOR 3
#define REG_COLOR 4
#define IO_COLOR 5
#define STACK_COLOR 6

#define CODE_WINDOW_WIDTH 60

WINDOW *io_win;
WINDOW *code_win;
WINDOW *mem_win;
WINDOW *reg_win;
WINDOW *stack_win;

void wait_input(char *msg)
{
	wattron(IO_WINDOW, COLOR_PAIR(MSG_COLOR) | A_BOLD);
	wprintw(IO_WINDOW, "%s", msg);
	wattroff(IO_WINDOW, COLOR_PAIR(MSG_COLOR) | A_BOLD);
	wrefresh(IO_WINDOW);
	wgetch(IO_WINDOW);
}

void panic(char *msg)
{
	wait_input(msg);
	wrefresh(IO_WINDOW);
	cleanup(2);
}

void debug_loop(char *fmt, arg_t location)
{
	wprintw(IO_WINDOW, fmt, location);
	wrefresh(IO_WINDOW);
}

void output(memory_t c)
{
	waddch(IO_WINDOW, c);
	wrefresh(IO_WINDOW);
}

memory_t input(void)
{
	return (memory_t)wgetch(IO_WINDOW);
	wrefresh(IO_WINDOW);
}

void cleanup(arg_t arg)
{
	endwin();
	free(memory);
	free(code);
	free(stack);
	free(bitcode);
	free(pstack);
	exit(arg);
}

void parse_argument(int argc, char **argv)
{
	FILE *corefile;
	int opt;

	if(!(argc >= 2))
	{
		puts("?ARG");
	}
	else
	{
		while((opt = getopt(argc, argv, "hmdf:c:s:t:b:")) != -1)
		{
			switch(opt)
			{
				case 's': /* Read size from argument */
					sscanf(optarg, "%zu,%zu,%zu,%zu", &memsize, &codesize, &pstacksize, &bitcodesize);
					if(memsize == 0 || codesize == 0 || stacksize == 0 || bitcodesize == 0)
						panic("?SIZE=0");
					free(memory);
					free(code);
					free(bitcode);
					free(pstack);
					memory	= calloc(memsize, sizeof(memory_t));
					code	= calloc(codesize, sizeof(code_t));
					pstack	= calloc(pstacksize, sizeof(memory_t));
					bitcode	= calloc(bitcodesize, sizeof(bitcode_t));
					break;
				case 'f': /* File */
					if((corefile = fopen(optarg, "r")) == NULL)
					{
						perror(optarg);
						exit(8);
					}
					read_code(code, corefile);
					fclose(corefile);
					break;
				case 'b':
					if(strcmp(optarg, "-"))
					{
						if((corefile = fopen(optarg, "r")) == NULL)
						{
							perror(optarg);
							exit(8);
						}
						bitcode_load_fp(bitcode, corefile);
						fclose(corefile);
					}
					else
						bitcode_load_fp(bitcode, stdin);
					load_bitcode=1;
					break;
				case 'c': /* Code */
					strncpy(code, optarg, codesize);
					break;
				case 't': /* Delay (in msec) */
					sscanf(optarg, "%d", &delay);
					delay *= 1000;
					break;
				case 'm':
					compat=1;
					break;
				case 'h': /* Help */
					printf("Usage: %s [-h] [-m] [-f file] [-c code] [-s memsize,codesize,pstacksize,bitcodesize] [-d] [-t msec]\n", argv[0]);
					break;
				case 'd': /* Debug */
					wprintw(IO_WINDOW, "DEBUG=1");
					wrefresh(IO_WINDOW);
					debug = TRUE;
					break;
				default:
					exit(1);

			}
		}
	}

}

void print_stack(memory_t *target, arg_t pointer)
{
	arg_t count=0;
	wclear(STACK_WINDOW);
	for(count=0; count <= pointer; count++)
	{
		if((pointer - count) < 13)
			wprintw(STACK_WINDOW, "stack[%4u] == %2x\n", count, target[count]);
	}
	wrefresh(STACK_WINDOW);
}

void print_reg(void)
{
	wclear(REG_WINDOW);
	wprintw(REG_WINDOW, "PTR       == %u\n", ptr);
	wprintw(REG_WINDOW, "PC        == %u\n", bitcode_ptr);
	wprintw(REG_WINDOW, "STACK_PTR == %u\n", pstack_ptr);
	wrefresh(REG_WINDOW);
}

void print_mem(void)
{
	int count;
	wclear(MEM_WINDOW);
	for(count=-8; count <= 7 ; count++)
	{
		if(((signed)ptr + count) < 0)
			waddstr(MEM_WINDOW, "    |");
		else if((ptr + count) >= memsize)
			waddstr(MEM_WINDOW, "    |");
		else
			wprintw(MEM_WINDOW, " %02x |", memory[ptr+count]);
	}
	for(count=0; count <= 7; count++)
	{
		waddstr(MEM_WINDOW, "     ");
	}
	wprintw(MEM_WINDOW, " ^^ PTR=%u", ptr);
	wrefresh(MEM_WINDOW);
}

void print_bitcode(arg_t ptr, arg_t *cursor)
{
	char temp_str[64];
	arg_t counter=0;

	if(ptr <= 5)
	{
		while(counter <= 5)
		{
			bitcode_disassembly(bitcode + counter, counter, temp_str, 64);
			wprintw(CODE_WINDOW, "%s\n", temp_str);
			counter++;
		}
		(*cursor)=ptr;
	}
	else
	{
		while(counter <= 5)
		{
			if((counter + ptr - 3) >= codesize)
				wprintw(CODE_WINDOW, "\n");
			else
			{
			bitcode_disassembly(bitcode + ptr + counter - 3, ptr + counter - 3, temp_str, 64);
			wprintw(CODE_WINDOW, "%s\n", temp_str);
			counter++;
			}
		}
		(*cursor)=counter - 3;
	}
	return;
}

void print_code(void)
{
	arg_t line;
	wclear(CODE_WINDOW);
	print_bitcode(bitcode_ptr, &line);
	mvwchgat(CODE_WINDOW, line, 0, -1, A_REVERSE, 0, NULL);
	wrefresh(CODE_WINDOW);
}

int main(int argc, char **argv)
{
	memory	= calloc(memsize, sizeof(memory_t));
	code	= calloc(codesize, sizeof(code_t));
	stack	= calloc(stacksize, sizeof(stack_type));
	bitcode	= calloc(bitcodesize, sizeof(bitcode_t));
	pstack	= calloc(pstacksize, sizeof(memory_t));

	initscr();

	if (! has_colors())
	{
		endwin();
		puts("?COLOR");
		exit(1);
	}

	start_color();

/* Window Layout
 *
 * 0		59	79
 * +-------------+-------+ 0
 * |         MEM         |	(BLACK on WHITE)
 * +-------------+-------+ 4
 * |             |       |
 * |    CODE     |  REG  |	(BG == YELLOW) : (BG == GREEN)
 * |             |       |
 * |-------------+-------+ 9
 * |             |       |
 * |     IO      | STACK |	(YELLOW / BLUE) : (BLACK / CYAN)
 * |             |       |
 * +-------------+-------+ 23
 */

	MEM_WINDOW	= newwin(4, 80, 0, 0);
	CODE_WINDOW	= newwin(6, 60, 4, 0);
	REG_WINDOW	= newwin(6, 20, 4, 60);
	IO_WINDOW       = newwin(13, 60, 10, 0);
	STACK_WINDOW	= newwin(13, 20, 10, 60);

	init_pair(MSG_COLOR, COLOR_BLACK, COLOR_WHITE);
	init_pair(MEM_COLOR, COLOR_BLACK, COLOR_WHITE);
	init_pair(CODE_COLOR, COLOR_BLACK, COLOR_YELLOW);
	init_pair(REG_COLOR, COLOR_BLACK, COLOR_GREEN);
	init_pair(IO_COLOR, COLOR_WHITE, COLOR_BLUE);
	init_pair(STACK_COLOR, COLOR_BLACK, COLOR_CYAN);

	parse_argument(argc, argv);
	wprintw(IO_WINDOW, "memory	= %p[%d]\n", memory, memsize);
	wprintw(IO_WINDOW, "code	= %p[%d]\n", code, codesize);
	wprintw(IO_WINDOW, "pstack	= %p[%d]\n", pstack, pstacksize);

	wbkgd(MEM_WINDOW, COLOR_PAIR(MEM_COLOR));
	wbkgd(CODE_WINDOW, COLOR_PAIR(CODE_COLOR));
	wbkgd(REG_WINDOW, COLOR_PAIR(REG_COLOR));
	wbkgd(IO_WINDOW, COLOR_PAIR(IO_COLOR));
	wbkgd(STACK_WINDOW, COLOR_PAIR(STACK_COLOR));

	wrefresh(MEM_WINDOW);
	wrefresh(CODE_WINDOW);
	wrefresh(REG_WINDOW);
	wrefresh(IO_WINDOW);
	wrefresh(STACK_WINDOW);

	wait_input("?START");
	wclear(IO_WINDOW);
	wrefresh(IO_WINDOW);

	scrollok(IO_WINDOW, TRUE);

	if(!load_bitcode)
		bitcodelize(bitcode, bitcodesize, code);

	while(TRUE)
	{
		print_code();
		print_reg();
		print_mem();
		if(delay)
			usleep(delay);
		bitcode_interprete(bitcode + bitcode_ptr);
	}
	wait_input("?HALT");
	cleanup(0);
	return 0;
}
