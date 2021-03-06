# Brainfunk
## Neo_Chen's Brainfuck Toolkit
### Passed all the tests

![Brainfunk Logo](https://gitlab.com/Neo_Chen/Brainfunk/raw/master/Logo/Logo256px.png "Yes, this is our logo")

Brainfunk is a simple interpreter, it have simple byte code,
but without dynamic memory allocation. VisualBrainfunk is just
like Brainfunk, but with ncurses based UI and Visualization.
They all have some simple array bounds checking, so normally it won't coredumps.

## Usage:
	brainfunk	[-h] [-m] [-f file] [-b bitcode] [-c code] [-d]
	visualbrainfunk	[-h] [-m] [-f file] [-b bitcode] [-c code] [-d] [-t msec]
	bf2bitcode	[-h] [-m] [-f infile] [-o outfile] [-c code] [-d]
	bf2c		[-h] [-m] [-f infile] [-b bitcode] [-o outfile] [-c code] [-d]

* -d is for debugging, super verbose
* -f is the file to read, "-" for reading from stdin
* -c is run code directly from the argument
* -s sets code, stack, memory, and bitcode size
* -t is delay between instructions, in millisecond
* -m is for compatibility with Brainfuck


## bf2c Usage:
Use bf2c to convert Brainfuck code to C, to compile it, add libstdbfc.h to compiler's search path (-I\<path\>)

## Brainfunk Instructions:
	+	Increment
	-	Decrement
	>	Forward
	<	Rewind
	[	Loop start
	]	Loop end
	.	Output
	,	Input
	\	Pop stack
	/	Push stack
	(	If start
	)	If end
	'??	Set hexdecimal
	$'??	Push hexdecimal
	$+	Increment top of stack
	$-	Decrement top of stack
	$[	while(peek()) {
	$]	}
	$(	if(peek()) {
	$)	}
	$.	Output from stack
	$,	Input to stack
	~	Fork
	!	Halt and catch fire!
	_	Halt
