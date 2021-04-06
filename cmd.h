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

extern Arg ss, dd, rnn;

extern Command command[];
extern Command cmd;

void do_halt();
void do_nothing();
void do_mov();
void do_add();
void do_sob();
void do_clr();
void do_inc();

#endif