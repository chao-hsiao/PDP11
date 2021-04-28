#include "cmd.h"
#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

Command command[] = {
	{0170000, 0010000, "MOV", HAS_DD + HAS_SS, do_mov},
	{0170000, 0110000, "MOVb", HAS_DD + HAS_SS + HAS_B, do_movb},
	{0170000, 0060000, "ADD", HAS_DD + HAS_SS, do_add},
	{0177000, 0077000, "SOB", HAS_R + HAS_NN, do_sob},
	{0177700, 0005200, "INC", HAS_DD, do_inc},
	//{0170000, 0020000, "CMP", HAS_DD + HAS_SS, do_cmp},
	//{0170000, 0120000, "CMPb", HAS_DD + HAS_SS + HAS_B, do_cmpb},
//	{0177700, 0005100, "COM", HAS_DD, do_com},
//	{0177700, 0105100, "COMb", HAS_DD + HAS_B, do_comb},
	{0177400, 0000400, "BR", HAS_XX, do_br},
	{0177400, 0001400, "BEQ", HAS_XX, do_beq},
	//{0177400, 0001000, "BNE", HAS_XX, do_bne},
	//{0177400, 0100400, "BMI", HAS_XX, do_bmi},
	{0177400, 0100000, "BPL", HAS_XX, do_bpl},
	//{0177400, 0002400, "BLT", HAS_XX, do_blt},
	//{0177400, 0002000, "BGE", HAS_XX, do_bge},
	//{0177400, 0003400, "BLE", HAS_XX, do_ble},
	{0177400, 0000100, "JMP", HAS_DD, do_jmp},
	{0177000, 0004000, "JSR", HAS_DD + HAS_R, do_jsr},
	{0177770, 0000200, "RTS", HAS_R, do_rts},
	{0177700, 0005700, "TST", HAS_DD, do_tst},
	{0177700, 0105700, "TSTb", HAS_DD + HAS_B, do_tstb},
	/*{0177777, 0000257, "CCC", NO_PARAMS, do_ccc},
	{0177777, 0000241, "CLC", NO_PARAMS, do_clc},
	{0177777, 0000250, "CLN", NO_PARAMS, do_cln},
	{0177777, 0000242, "CLV", NO_PARAMS, do_clv},
	{0177777, 0000244, "CLZ", NO_PARAMS, do_clz},
	{0177777, 0000244, "SCC", NO_PARAMS, do_scc},
	{0177777, 0000244, "SEC", NO_PARAMS, do_sec},
	{0177777, 0000244, "SEN", NO_PARAMS, do_sen},
	{0177777, 0000244, "SEV", NO_PARAMS, do_sev},
	{0177777, 0000244, "SEZ", NO_PARAMS, do_sez},
	{0177777, 0000240, "NOP", NO_PARAMS, do_nop},*/
	{0177700, 0005000, "CLR", HAS_DD, do_clr},
	{0177777, 0000000, "HALT", NO_PARAMS, do_halt},
	{0000000, 0000000, "NOTHING", 0, do_nothing} // MUST LAST
};

Command cmd;
Arg b, ss, dd, nn, n, r, tt, xx;

char * NZVC (word w);
void trace2();

void do_add() {
	w_write(dd.adr, ss.val + dd.val, in_reg(dd.adr));
	PSW = psw(ss.val, dd.val);
	if (in_reg(dd.adr))
		trace(TRACE1, "\t  R%o=%06o R%o=%06o\n", ss.adr, ss.adr == dd.adr ? w_read(ss.adr, in_reg(ss.adr)) / 2 : w_read(ss.adr, in_reg(ss.adr)), dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	else
		trace(TRACE1, "[%06o]=%06o [%06o]=%06o\n", ss.adr, w_read(ss.adr, in_reg(ss.adr)), dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	trace2();
}

void do_sob() {
	if (--reg[r.adr] != 0){
		pc = pc - 2 * nn.val;
		trace(TRACE1, "R%o,%06o \n", r.adr, pc);
	}
	else
		trace(TRACE1, "R%o,%06o \n", r.adr, pc - 2 * nn.val);
	trace2();
}

void do_inc() {
	w_write(dd.adr, dd.val + 1, in_reg(dd.adr));
	PSW = psw(dd.val, 1) | (PSW & 1);
	if (in_reg(dd.adr))
		trace(TRACE1, "\t  R%o=%06o\n", dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	else
		trace(TRACE1, "\t[%06o]=%06o\n", dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	trace2();
}

void do_clr() {
	reg[dd.adr] = 0;
	PSW = psw(reg[dd.adr], 0);
	trace(TRACE1, "\n");
	trace2();
}

void do_mov() {
	w_write(dd.adr, ss.val, in_reg(dd.adr));
	PSW = psw(ss.val, 0) | (PSW & 1);
	if(in_reg(ss.adr)){
		trace(TRACE1, "R%o=%06o", ss.adr, w_read(ss.adr, in_reg(ss.adr)));
		if(dd.adr == odata){
			trace(TRACE1, " [%06o] %c ", dd.adr, w_read(dd.adr, in_reg(dd.adr)));
			w_write(odata, 0000000, in_reg(odata));
		}
	}
	else{
		trace(TRACE1, "[%06o]=%06o", ss.adr, w_read(ss.adr, in_reg(ss.adr)));
		if(dd.adr == odata){
			trace(TRACE1, " [%06o] %c ", dd.adr, b_read(dd.adr, in_reg(dd.adr)));
			w_write(odata, 0000000, in_reg(odata));
		}
	}
	trace(TRACE1, "\n");
	trace2();
}
void do_movb() {
	b_write(dd.adr, ss.val, in_reg(dd.adr));
	PSW = psw(ss.val >> 7 ? ss.val | 0xff00 : ss.val, 0) | (PSW & 1);
	if(in_reg(ss.adr)){
		trace(TRACE1, "R%o=%03o", ss.adr, b_read(ss.adr, in_reg(ss.adr)));
		if(dd.adr == odata){
			trace(TRACE1, " [%06o] %c ", dd.adr, b_read(dd.adr, in_reg(dd.adr)));
			w_write(odata, 0000000, in_reg(odata));
		}
	}
	else{
		trace(TRACE1, "[%06o]=%03o", ss.adr, b_read(ss.adr, in_reg(ss.adr)));
		if(dd.adr == odata){
			trace(TRACE1, " [%06o] %c ", dd.adr, b_read(dd.adr, in_reg(dd.adr)));
			w_write(odata, 0000000, in_reg(odata));
		}
	}
	trace(TRACE1, "\n");
	trace2();
}

void do_halt() {
	trace(TRACE1, "THE_END\n");
	trace2();
	printf("\n---------------- halted --------------\n");
	printf("R0=%06o R2=%06o R4=%06o SP=%06o\n", reg[0],reg[2],reg[4],reg[6]);
	printf("R1=%06o R3=%06o R5=%06o PC=%06o\n", reg[1],reg[3],reg[5],reg[7]);
	char * nzvc = NZVC(PSW);
	printf("psw=%06o: cm=k pm=k pri=0   %s [%d]\n", PSW, nzvc, counter);
	free(nzvc);
	exit(0);
}

void do_br(){
	pc = (pc + 2 * xx.adr) & 0xffff;
	trace(TRACE1, "%06o\n", pc);
	trace2();
}
void do_beq(){
	trace(TRACE1, "%06o\n", (pc + 2 * xx.adr) & 0xffff);
	if (PSW & 4)	//if z = 1
		pc = (pc + 2 * xx.adr) & 0xffff;	//do_br()
	trace2();
}
void do_bpl(){
	trace(TRACE1, "%06o\n", (pc + 2 * xx.adr) & 0xffff);	
	if ((PSW & 8) == 0)	//if n = 0
		pc = (pc + 2 * xx.adr) & 0xffff;	//do_br()
	trace2();
}
void do_tst(){}
void do_tstb(){
	PSW = psw(dd.val >> 7 ? dd.val | 0xff00 : dd.val, 0);
	if (in_reg(dd.adr))
		trace(TRACE1, "R%o=%03o\n", dd.adr, b_read(dd.adr, in_reg(dd.adr)));
	else
		trace(TRACE1, "[%06o]=%03o\n", dd.adr, b_read(dd.adr, in_reg(dd.adr)));
	trace2();
}

void do_rts(){
	trace(TRACE1,"\n");
	pc = reg[r.adr];
	reg[r.adr] = w_read(sp, in_reg(sp));
	sp += 2;
	trace2();
}
void do_jsr(){
	sp -= 2;
	w_write(sp, reg[r.adr], in_reg(sp));
	reg[r.adr] = pc;
	pc = dd.adr;
	trace(TRACE1,"\n");
	trace2();
}
void do_jmp(){
	pc = dd.adr;
	trace2();
}
/*
void do_com(){
	dd.adr = ~ dd.adr;
	PSW = psw(dd.adr, 0) | 1;
	trace2();
}
void do_cobm(){
	dd.adr = ~ dd.adr;
}
*/
void do_nothing() {
	trace(TRACE1, "\n");
}

word psw(word w0, word w1){
	word res = 0;
	if ((w0 + w1) >> 15 & 1)
		res = res + 8;
	if (((w0 + w1) & 0177777) == 0)
		res = res + 4;
	if (w0 >> 15 == w1 >> 15){
		if ((((w0 + w1) >> 15) & 1) != w0 >> 15)
			res = res + 2;
	}
	if ((w0 + w1) >> 16)
		res = res + 1;

	return res;
}

char * NZVC (word w) {
	char * res = malloc(5 * sizeof(char));
	res[4] = '\0';

	if (w & 1)
		res[3] = 'C';
	else
		res[3] = ' ';

	if (w & 2)
		res[2] = 'V';
	else
		res[2] = ' ';

	if (w & 4)
		res[1] = 'Z';
	else
		res[1] = ' ';

	if (w & 8)
		res[0] = 'N';
	else
		res[0] = ' '; 

	return res;
}

void trace2(){
	trace(TRACE2, "---- 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
	//printf("psw=%06o: [%d]\n", PSW, counter);
}





