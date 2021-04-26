#include "cmd.h"
#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

Command command[] = {
	{0170000, 0010000, "MOV", 3, do_mov},
	{0170000, 0110000, "MOVb", 3 , do_movb},
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
	w_write(dd.adr, ss.val + dd.val, in_reg(dd.adr));
	if (in_reg(dd.adr))
		trace(TRACE1, " \t\t\tR%o=%06o R%o=%06o\n", ss.adr, w_read(ss.adr, in_reg(ss.adr)), dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	else
		trace(TRACE1, " \t\t[%06o]=%06o [%06o]=%06o\n", ss.adr, w_read(ss.adr, in_reg(ss.adr)), dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
}

void do_sob() {
	if (--reg[rnn.adr] != 0){
		pc = pc - 2 * rnn.val;
		trace(TRACE1, "%06o \n", pc);
	}
	else
		trace(TRACE1, "%06o \n", pc - 2 * rnn.val);
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
}

void do_inc() {}

void do_clr() {
	reg[dd.adr] = 0;
	trace(TRACE1, "\n");
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
}

void do_mov() {
	w_write(dd.adr, ss.val, in_reg(dd.adr));
	trace(TRACE1, " \t\t[%06o]=%06o\n", ss.adr, w_read(ss.adr, in_reg(ss.adr)));
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
}
void do_movb() {
	if(in_reg(dd.adr))
		w_write(dd.adr, ss.val >> 7 ? (ss.val & 0xff) | 0xff00 : ss.val & 0xff, in_reg(dd.adr));
	else
		b_write(dd.adr, ss.val, in_reg(dd.adr));

	trace(TRACE1, " \t\t[%06o]=%03o\n", ss.adr, b_read(ss.adr, in_reg(ss.adr)));
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
}

void do_halt() {
	trace(TRACE1, "THE_END\n");
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
	printf("\n---------------- halted --------------\n");
	printf("R0=%06o R2=%06o R4=%06o SP=%06o\n", reg[0],reg[2],reg[4],reg[6]);
	printf("R1=%06o R3=%06o R5=%06o PC=%06o\n", reg[1],reg[3],reg[5],reg[7]);
	printf("psw=%06o: cm=k pm=k pri=0   NZVC [%d]\n", PSW, counter);
	exit(0);
}

void do_nothing() {}








