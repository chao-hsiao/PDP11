/*
#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>

word reg[8];
reg[7] = 01000;
#define pc reg[7]


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

	while(1) {
		word w = w_read(pc);
		printf("%06o\n", pc);
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
*/
#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define pc reg[7]

void run();
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

Command command[] = {
	{0170000, 0010000, "mov", do_mov},
	{0170000, 0060000, "add", do_add},
	{0177777, 0000000, "halt", do_halt},
	//{0177700, 0005200, "inc", do_inc}
	{0000000, 0000000, "nothing", do_nothing} // MUST LAST
};

typedef struct {
	word val;
	word adr;
} Argument;

Argument ss, dd;

//enum LOGLEVEL current_log_level = INFO;
int current_log_level = DEBUG;

void usage(const char * filename) {
	printf("USAGE: %s pdp_filename\n", filename);
}

int main(int argc, char const *argv[]) {

	if (argc <= 1) {
		usage(argv[0]);
		return 1;
	}

	load_file(argv);
	//mem_dump(0x200,0xc);
	run();
	/*
	int x = 4;
    trace(DEBUG, "My debug info x=%d\n", x);
	x = 6;
    trace(TRACE, "trace x=%d\n", x);
	trace(INFO, "hello\n");
	trace(ERROR, "ERROR: %s\n", command[0].name);
	int old_log_level = current_log_level;
	current_log_level = DEBUG;
    trace(DEBUG, "My debug info x=%d\n", x);
	current_log_level = old_log_level;
	*/
	return 0;
}

void run() {
	pc = 01000;

	while(1) {
		word w = w_read(pc);
		trace(TRACE, "%06o %06o: ", pc, w);
		pc = pc + 2;

		int i;
		for(i = 0; ; i++) {
			Command cmd = command[i];
			if ((w & cmd.mask) == cmd.opcode) {
				trace(TRACE, "%s ", cmd.name);
				cmd.do_func();
				trace(TRACE, "\n");
				break;
			}
		}
	}
}

void do_add() {
}

void do_mov() {
	w_write(dd.adr, ss.val);
}

void do_halt() {
	printf("THE END\n");
	exit(0);
}

void do_nothing() {
  
}

void trace(int log_level, const char * format, ...){
	// никакой печати, если логирование ниже по уровню
	if (log_level < current_log_level)
		return;
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}
