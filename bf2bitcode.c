/* ==================================== *
 * Brainfunk -- A Brainf**k Toolkit     *
 * Neo_Chen			        *
 * ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <libbrainfunk.h>

memory_t *memory;
arg_t ptr=0;
stack_type *stack;
arg_t stack_ptr=0;
code_t *code;
arg_t code_ptr=0;
bitcode_t *bitcode;
arg_t bitcode_ptr=0;
memory_t *pstack;
arg_t pstack_ptr=0;

size_t memsize=DEF_MEMSIZE;
size_t codesize=DEF_CODESIZE;
size_t stacksize=DEF_STACKSIZE;
size_t bitcodesize=DEF_BITCODESIZE;
size_t pstacksize=DEF_PSTACKSIZE;

int debug=0;
int compat=0;

FILE *corefile=NULL;
FILE *outfile=NULL;

void cleanup(arg_t arg)
{
	fclose(outfile);
	free(bitcode);
	free(stack);
	free(code);
	exit(arg);
}

int main(int argc, char **argv)
{
	code	= calloc(codesize, sizeof(code_t));
	bitcode	= calloc(bitcodesize, sizeof(bitcode_t));
	stack	= calloc(stacksize, sizeof(stack_type));

	int opt;
	outfile=stdout;

	if(argc <= 2)
	{
		panic("?ARG");
	}
	else
	{
		while((opt = getopt(argc, argv, "hdmf:s:c:o:")) != -1)
		{
			switch(opt)
			{
				case 'd':
					debug=1;
					break;
				case 's': /* Read size from argument */
					sscanf(optarg, "%zu,%zu,%zu", &codesize, &stacksize, &bitcodesize);
					if(codesize == 0 || stacksize == 0 || bitcodesize == 0)
						panic("?SIZE=0");
					free(memory);
					free(code);
					free(stack);
					code	= calloc(codesize, sizeof(code_t));
					stack	= calloc(stacksize, sizeof(stack_type));
					bitcode	= calloc(bitcodesize, sizeof(bitcode_t));
					break;
				case 'c':
					strncpy(code, optarg, codesize);
					break;
				case 'f':
					if(strcmp(optarg, "-"))
					{
						if((corefile = fopen(optarg, "r")) == NULL)
						{
							perror(optarg);
							exit(8);
						}
						read_code(code, corefile);
						fclose(corefile);
					}
					else
						read_code(code, stdin);
					break;
				case 'o':
					if(strcmp(optarg, "-"))
					{
						if((outfile = fopen(optarg, "w")) == NULL)
						{
							perror(optarg);
							exit(8);
						}
					}
					else
						outfile=stdout;
					break;
				case 'm':
					compat=1;
					break;
				case 'h':
				default:
					printf("Usage: %s [-h] [-m] [-b bitcode] -f infile -o outfile [-s codesize,stacksize,bitcodesize] [-d]\n", argv[0]);
					exit(1);
			}
		}
	}

	bitcodelize(bitcode, bitcodesize, code);

	char outstr[128];
	do
	{
		bitcode_disassembly(bitcode + bitcode_ptr, bitcode_ptr, outstr, 128);
		fprintf(outfile, "%s\n", outstr);
	}
	while((bitcode + bitcode_ptr++)->op != OP_HLT);
	cleanup(0);
}
