#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define pc reg[7]

//enum LOGLEVEL current_log_level = INFO;
int current_log_level = TRACE;

void run();
void do_halt(word w);
void do_nothing(word w);
void do_mov(word w);
void do_add(word w);
//void do_inc();

typedef struct {
	word mask;
	word opcode;
	char * name;
	void (*do_func)(word w);
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
} Arg;

Arg ss, dd;
Arg get_modereg(word w);

void usage(const char * filename) {
	printf("USAGE: %s pdp_filename\n", filename);
}

int main(int argc, char const *argv[]) {

	printf("\n---------------- running --------------\n");

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

		for(int i = 0; ; i++) {
			Command cmd = command[i];
			if ((w & cmd.mask) == cmd.opcode) {
				trace(TRACE, "%s ", cmd.name);
				cmd.do_func(w);
				trace(TRACE, "\n");
				break;
			}
		}
	}
}

void do_add(word w) {
}

void do_mov(word w) {
	//word w = w_read(pc - 2);	//you have to like that cuz after first get_modered the valuse of r7(pc) may changed
	ss = get_modereg(w >> 6);
	printf(",");
	dd = get_modereg(w);
	w_write(dd.adr, ss.val);
	//trace(TRACE, "%o", dd.val);
}

void do_halt(word w) {
	printf("THE END\n\n");
	exit(0);
}

void do_nothing(word w) {
  
}

Arg get_modereg(word w) {
	Arg res;
	word regi, mode, index;
	trace(DEBUG,"%o ", w);
	regi = w & 07;
	trace(DEBUG,"%o ", regi);
	mode = w >> 3 & 07;
	trace(DEBUG,"%o ", mode);
	reg[6] = 01000;

	switch (mode) {
		case 0:				//Rn
			res.adr = regi;
			res.val = reg[regi];
			if (regi == 7)
				trace(TRACE, "pc");
			else
				trace(TRACE, "R%o", regi);
			break;
		case 1:				//(Rn)
			res.adr = reg[regi];
			res.val = res.adr % 2 ? b_read(res.adr) : w_read(res.adr);
			if (regi == 7)
				trace(TRACE, "(pc)", regi);
			else
				trace(TRACE, "(R%o)", regi);
			break;
		case 2:				//(Rn)+; if n = 7 -> #val
			res.adr = reg[regi];
			res.val = res.adr % 2 ? b_read(res.adr) : w_read(res.adr);
			reg[regi] = reg[regi] + 2;
			if (regi == 7)
				trace(TRACE, "#%o", res.val);
			else
				trace(TRACE, "(R%o)+", regi);
			break;
		case 3:				//@(Rn)+; if n = 7 -> @#adr
			res.adr = reg[regi] % 2 ? b_read(reg[regi]) : w_read(reg[regi]);
			res.val = res.adr % 2 ? b_read(res.adr) : w_read(res.adr);
			reg[regi] = reg[regi] + 2;
			if (regi == 7)
				trace(TRACE, "@#%o", res.adr);
			else
				trace(TRACE, "@(R%o)+", regi);
			break;
		case 4:				//-(Rn)
			reg[regi] = reg[regi] - 2;
			res.adr = reg[regi];
			res.val = res.adr % 2 ? b_read(res.adr) : w_read(res.adr);	
			if (regi == 7)
				trace(TRACE, "-(pc)");
			else
				trace(TRACE, "-(R%o)", regi);
			break;
		case 5:				//@-(Rn)
			reg[regi] = reg[regi] - 2;
			res.adr = reg[regi] % 2 ? b_read(reg[regi]) : w_read(reg[regi]);
			res.val = res.adr % 2  ? b_read(res.adr) : w_read(res.adr);
			if (regi == 7)
				trace(TRACE, "@-(pc)");
			else
				trace(TRACE, "@-(R%o)", regi);
			break;
		case 6:				//X(Rn); if n = 7 -> adr
			index = w_read(pc);
			pc = pc + 2;
			res.adr = reg[regi] + index;
			res.val = res.adr % 2 ? b_read(res.adr) : w_read(res.adr);
			if (regi == 7)
				trace(TRACE, "%o ", res.adr);
			else
				trace(TRACE, "%o(R%o)", index, regi);
			break;
		case 7:				//@X(Rn); if n = 7 -> @adr
			index = w_read(pc);
			pc = pc + 2;
			res.adr = ((reg[regi] + index) % 2) == 1 ? b_read(reg[regi] + index) : w_read(reg[regi] + index);
			res.val = res.adr % 2 ? b_read(res.adr) : w_read(res.adr);
			if (regi == 7)
				trace(TRACE, "@%o", res.adr);
			else
				trace(TRACE, "@%o(R%o)", index, regi);
			break;
		//default:
			//fprintf("%s\n", );
			//exit(1);
	}

	return res;
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
