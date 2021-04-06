#include "cmd.h"
#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

Command command[] = {
	{0170000, 0010000, "MOV", 3, do_mov},
	{0170000, 0060000, "ADD", 3, do_add},
	{0177000, 0077000, "SOB", 12, do_sob},
	{0177700, 0005000, "CLR", 1, do_clr},
	{0177777, 0000000, "HALT", 0, do_halt},
	{0177700, 0005200, "INC", 1, do_inc},
	{0000000, 0000000, "NOTHING", 0, do_nothing} // MUST LAST
};

Command cmd;
Arg ss, dd, rnn;

void do_add() {
	reg[dd.adr] = reg[dd.adr] + reg[ss.adr];
	printf(" %o", reg[dd.adr]);
}

void do_sob() {
	reg[rnn.adr] = reg[rnn.adr] - 1;
	if (reg[rnn.adr] != 0)
		pc = pc - 2 * rnn.val;
	//goto NN;
	printf(" %o ", pc);
}

void do_inc() {

}

void do_clr() {
	reg[dd.adr] = 0;
}

void do_mov() {
	//word w = w_read(pc - 2);	//you have to like that cuz after first get_modered the valuse of r7(pc) may changed
	w_write(dd.adr, ss.val);
	//trace(TRACE, "%o", dd.val);
}

void do_halt() {
	printf("THE END\n\n");
	exit(0);
}

void do_nothing() {
  
}









