/* ==================================== *
 * Brainfunk -- A Brainf**k Toolkit     *
 * Neo_Chen			        *
 * ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define DEF_MEMSIZE 262144
#define DEF_CODESIZE 65536
#define DEF_STACKSIZE 1024
#define DEF_BITCODESIZE 65536
#define DEF_PSTACKSIZE	65536

#define TRUE 1
#define FALSE 0

enum opcodes
{
	OP_HLT,
	OP_ADD,
	OP_SUB,
	OP_FWD,
	OP_REW,
	OP_JEZ,
	OP_JNZ,
	OP_IO,
	OP_SET,
	OP_POP,
	OP_PUSH,
	OP_PSHI,
	OP_ADDS,
	OP_SUBS,
	OP_JMP,
	OP_JSEZ,
	OP_JSNZ,
	OP_FRK,
	OP_HCF,
	OP_INSTS /* Total number of instructions */
};

#define ARG_IN	0
#define ARG_OUT	1
#define ARG_INS	2
#define ARG_OUTS	3

struct bitcode_struct
{
	unsigned char op;
	unsigned int arg;
};

struct bitcode_ref_s
{
	char name[16];
	char format[128];
	char cformat[128];
	void(*handler)(unsigned int);
};

struct bitcode_ref_s bitcode_ref[OP_INSTS];
char vaild_code[256];

typedef struct bitcode_struct bitcode_t;
typedef uint8_t memory_t;
typedef unsigned int stack_type;
typedef unsigned int arg_t;
typedef char code_t;

void panic(char *msg);
void read_code(char *code, FILE* fp);
void push(stack_type *stack, unsigned int *ptr, stack_type content);
stack_type pop(stack_type *stack, unsigned int *ptr);
void debug_output(void);
void jump_to_next_matching(void);
void interprete(code_t c);
void output(memory_t c);
memory_t input(void);
void debug_loop(char *fmt, unsigned int location);
int is_code(int c);

#ifdef VISUAL
void print_stack(stack_type *target, unsigned int pointer);
#endif

void bitcodelize(bitcode_t *bitcode, size_t bitcodesize, code_t *text);
void bitcode_interprete(bitcode_t *bitcode);
void bitcode_disassembly(bitcode_t *bitcode, unsigned int address, char *str, size_t strsize);
void bitcode_disassembly_array_to_fp(bitcode_t *bitcode, FILE *fp);
void bitcode_assembly(char *str, bitcode_t *bitcode);
void bitcode_load_fp(bitcode_t *bitcode, FILE *fp);

void exec_hlt(arg_t arg);
void exec_add(arg_t arg);
void exec_adds(arg_t arg);
void exec_sub(arg_t arg);
void exec_subs(arg_t arg);
void exec_fwd(arg_t arg);
void exec_rew(arg_t arg);
void exec_jez(arg_t arg);
void exec_jsez(arg_t arg);
void exec_jnz(arg_t arg);
void exec_jsnz(arg_t arg);
void exec_jmp(arg_t arg);
void exec_set(arg_t arg);
void exec_push(arg_t arg);
void exec_pop(arg_t arg);
void exec_pshi(arg_t arg);
void exec_io(arg_t arg);
void exec_frk(arg_t arg);
void exec_hcf(arg_t arg);

void cleanup(arg_t arg);
