# Brainfunk
## Neo_Chen's Simple Brainfuck Interpreter
### Passed all the tests
Brainfunk is a simple speed-orientated interpreter, it
doesn't have any fancy things like Byte Code (currently),
and dynamic memory allocation. VisualBrainfunk is just
like Brainfunk, but with ncurses based UI and Visualization.
Bitfunk is basically Brainfunk with bytecode and deduplication optimize.

They all have some simple array bounds checking, so normally it won't coredumps.

## Usage:
	brainfunk [-h] [-f file] [-c code] [-d]
	bitfunk [-h] [-f file] [-c code] [-d]
	visualbrainfunk [-h] [-f file] [-c code] [-d] [-t msec]
	visualbitfunk [-h] [-f file] [-c code] [-d] [-t msec]
	bf2bitcode [-h] [-f infile] [-o outfile] [-c code]

* -d is for debugging, super verbose
* -f is the file to read, "-" for reading from stdin
* -c is run code directly from the argument
* -s sets code, stack, memory, and bitcode size
* -t is delay between instructions, in millisecond
