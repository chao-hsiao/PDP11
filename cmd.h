#ifndef CMD_H
#define CMD_H
#include "pdp11.h"

typedef struct {
	word mask;
	word opcode;
	char * name;
	char params;
	void (*do_func)(void);
} Command;

typedef struct {
	word val;
	word adr;
} Arg;

extern Arg b, ss, dd, nn, n, r, tt, xx;

extern Command command[];
extern Command cmd;

word psw(word w0, word w1);

void do_halt();
void do_nothing();
void do_mov();
void do_movb();
void do_add();
void do_sob();
void do_clr();
void do_inc();
void do_br();
void do_beq();

#endif