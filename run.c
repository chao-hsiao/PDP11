#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>

word reg[8];

//#define pc reg[7]

void run();
void trace(char * c, word pc, word w);
void do_halt();
void do_nothing();
void do_mov();
void do_add();
//void do_inc();

typedef struct {
	word mask;
	word opcode;
	char * name;
	void (*do_func)();
} Command;

Command cmd[] = {
	{0170000, 0010000, "mov", do_mov},
	{0170000, 0060000, "add", do_add},
	{0111111, 0000000, "halt", do_halt},
	//{0177700, 0005200, "inc", do_inc}
};

int main(int argc, char const *argv[]) {

	load_file(argv[1]);
	//mem_dump(0x200,0xc);
	run();
    

	return 0;
}

void run() {
	word pc = reg[7];
	pc = 01000;

	while(1) {
		word w = w_read(pc);
		trace("%06o %06o: ", pc, w);
		pc = pc + 2;

		if (w == 0) {
			printf("%s\n", cmd[2].name);
			cmd[2].do_func();
		}

		else if ((w & cmd[0].mask) == cmd[0].opcode) {
			cmd[0].do_func();
		}
		else if ((w & cmd[1].mask) == cmd[1].opcode) {
			cmd[1].do_func();
		}
		else
			do_nothing();
	}

}

void do_add() {
	printf("%s\n", cmd[1].name);
}

void do_mov() {
	printf("%s\n", cmd[0].name);
}

void do_halt() {
	printf("THE END\n");
	exit(0);
}

void do_nothing() {
	printf("unknown\n");
}

void trace(char * c, word pc, word w) {
	printf(c, pc, w);
}

