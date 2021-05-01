#include "cmd.h"
#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

Command command[] = {
	{0170000, 0010000, "MOV", HAS_DD | HAS_SS, do_mov},				//dd+ss //0
	{0170000, 0110000, "MOVb", HAS_DD | HAS_SS | HAS_B, do_movb},
	{0170000, 0060000, "ADD", HAS_DD | HAS_SS, do_add},
	{0170000, 0160000, "SUB", HAS_DD | HAS_SS, do_sub},
	{0170000, 0020000, "CMP", HAS_DD | HAS_SS, do_cmp},
	{0170000, 0120000, "CMPb", HAS_DD | HAS_SS | HAS_B, do_cmpb},
	{0170000, 0040000, "BIC", HAS_DD | HAS_SS, do_bic},
	{0170000, 0140000, "BICb", HAS_DD | HAS_SS | HAS_B, do_bicb},
	{0170000, 0050000, "BIS", HAS_DD | HAS_SS, do_bis},
	{0170000, 0150000, "BISb", HAS_DD | HAS_SS | HAS_B, do_bisb},
	{0170000, 0030000, "BIT", HAS_DD | HAS_SS, do_bit},
	{0170000, 0130000, "BITb", HAS_DD | HAS_SS | HAS_B, do_bitb},			//11

	{0177000, 0070000, "MUL", HAS_R | HAS_SS, do_mul},				//r+ss  //12
	{0177000, 0071000, "DIV", HAS_R | HAS_SS, do_div},
	{0177000, 0072000, "ASH", HAS_R | HAS_SS, do_ash},
	{0177000, 0073000, "ASHC", HAS_R | HAS_SS, do_ashc},
	{0177000, 0073000, "XOR", HAS_R | HAS_SS, do_xor},						//16

	{0177400, 0000100, "JMP", HAS_DD, do_jmp},						//dd 	//17
	{0177700, 0006100, "ROL", HAS_DD, do_rol},
	{0177700, 0106100, "ROLb", HAS_DD | HAS_B, do_rolb},
	{0177700, 0006000, "ROR", HAS_DD, do_ror},
	{0177700, 0106000, "RORb", HAS_DD | HAS_B, do_rorb},
	{0177700, 0005100, "COM", HAS_DD, do_com},
	{0177700, 0105100, "COMb", HAS_DD | HAS_B, do_comb},
	{0177400, 0005500, "ADC", HAS_DD, do_adc},
	{0177400, 0105500, "ADCb", HAS_DD | HAS_B, do_adcb},
	{0177400, 0006300, "ASL", HAS_DD, do_asl},
	{0177400, 0106300, "ASLb", HAS_DD | HAS_B, do_aslb},
	{0177400, 0006200, "ASR", HAS_DD, do_asr},
	{0177400, 0106200, "ASRb", HAS_DD | HAS_B, do_asrb},
	{0177700, 0005000, "CLR", HAS_DD, do_clr},
	{0177700, 0105000, "CLRb", HAS_DD | HAS_B, do_clrb},
	{0177700, 0005700, "TST", HAS_DD, do_tst},
	{0177700, 0105700, "TSTb", HAS_DD | HAS_B, do_tstb},
	{0177400, 0005300, "DEC", HAS_DD, do_dec},
	{0177400, 0105300, "DECb", HAS_DD | HAS_B, do_decb},
	{0177700, 0005200, "INC", HAS_DD, do_inc},
	{0177700, 0105200, "INCb", HAS_DD | HAS_B, do_incb},
	{0177400, 0005400, "NEG", HAS_DD, do_neg},
	{0177400, 0105400, "NEGb", HAS_DD | HAS_B, do_negb},
	{0177400, 0005600, "SBC", HAS_DD, do_sbc},
	{0177400, 0105600, "SBCb", HAS_DD | HAS_B, do_sbcs},
	{0177400, 0000300, "SWAB", HAS_DD, do_swab},
	{0177400, 0006700, "SXT", HAS_DD, do_sxt},								//43
	{0177400, 0106400, "MTPS", HAS_DD, do_mtps},
	{0177400, 0006500, "MFPI", HAS_DD, do_mfpi},
	{0177400, 0106500, "MFPD", HAS_DD, do_mfpd},
	{0177400, 0006600, "MTPI", HAS_DD, do_mtpi},
	{0177400, 0106600, "MTPD", HAS_DD, do_mtpd},							//48

	{0177400, 0000400, "BR", HAS_XX, do_br},						//xx 	//49
	{0177400, 0001400, "BEQ", HAS_XX, do_beq},
	{0177400, 0001000, "BNE", HAS_XX, do_bne},
	{0177400, 0100400, "BMI", HAS_XX, do_bmi},
	{0177400, 0100000, "BPL", HAS_XX, do_bpl},
	{0177400, 0002400, "BLT", HAS_XX, do_blt},
	{0177400, 0002000, "BGE", HAS_XX, do_bge},
	{0177400, 0003400, "BLE", HAS_XX, do_ble},
	{0177400, 0103000, "BCC", HAS_XX, do_bcc},
	{0177400, 0103400, "BCS", HAS_XX, do_bcs},
	{0177400, 0003000, "BGT", HAS_XX, do_bgt},
	{0177400, 0101000, "BHI", HAS_XX, do_bhi},
	{0177400, 0103000, "BHIS", HAS_XX, do_bhis},
	{0177400, 0103400, "BLO", HAS_XX, do_blo},
	{0177400, 0101400, "BLOS", HAS_XX, do_blos},
	{0177400, 0102000, "BVC", HAS_XX, do_bvc},
	{0177400, 0102400, "BVS", HAS_XX, do_bvs},								//65

	{0177000, 0077000, "SOB", HAS_R | HAS_NN, do_sob},				//r+nn 	//66

	{0177000, 0004000, "JSR", HAS_R | HAS_DD, do_jsr},				//r+dd  //67

	{0177770, 0000200, "RTS", HAS_R, do_rts},								//68

	{0177777, 0000257, "CCC", NO_PARAMS, do_ccc},					//nzvc
	{0177777, 0000241, "CLC", NO_PARAMS, do_clc},
	{0177777, 0000250, "CLN", NO_PARAMS, do_cln},
	{0177777, 0000242, "CLV", NO_PARAMS, do_clv},
	{0177777, 0000244, "CLZ", NO_PARAMS, do_clz},
	{0177777, 0000244, "SCC", NO_PARAMS, do_scc},
	{0177777, 0000244, "SEC", NO_PARAMS, do_sec},
	{0177777, 0000244, "SEN", NO_PARAMS, do_sen},
	{0177777, 0000244, "SEV", NO_PARAMS, do_sev},
	{0177777, 0000244, "SEZ", NO_PARAMS, do_sez},
	{0177777, 0000240, "NOP", NO_PARAMS, do_nop},

	{0177777, 0000000, "HALT", NO_PARAMS, do_halt},
	{0177777, 0000001, "WAIT", NO_PARAMS, do_wait},
	{0177777, 0000005, "RESET", NO_PARAMS, do_reset},

	{0000000, 0000000, "NOTHING", NO_PARAMS, do_nothing} // MUST LAST
};

Command cmd;
Arg ss, dd, nn, n, r, tt;

void do_mov() {
	w_write(dd.adr, ss.val, in_reg(dd.adr));
	PSW = psw(ss.val, 0) | (PSW & 1);
	if(in_reg(ss.adr))
		trace(TRACE1, "\t  R%o=%06o", ss.adr, w_read(ss.adr, in_reg(ss.adr)));
	else
		trace(TRACE1, "[%06o]=%06o", ss.adr, w_read(ss.adr, in_reg(ss.adr)));

	if(in_reg(dd.adr) == 0)
		trace(TRACE1, " [%06o]", dd.adr);
	if(dd.adr == odata){					//output
		if(current_log_level == INFO){
			if (!isatty(fileno(stdout)))
				fprintf(stdout, "%c", w_read(dd.adr, in_reg(dd.adr)));
			else
				fprintf(stderr, "%c", w_read(dd.adr, in_reg(dd.adr)));
		}
		else{
			trace(TRACE1, " [%06o] ", dd.adr);
			if (!isatty(fileno(stdout)))
				fprintf(stdout, "%c", w_read(dd.adr, in_reg(dd.adr)));
			else
				trace(TRACE1, "%c", w_read(dd.adr, in_reg(dd.adr)));
		}
		//w_write(odata, 0000000, in_reg(odata));
	}

	trace(TRACE1, "\n");
	trace2();
}
void do_movb() {
	b_write(dd.adr, ss.val, in_reg(dd.adr));
	PSW = psw(ss.val >> 7 ? ss.val | 0xff00 : ss.val, 0) | (PSW & 1);
	if(in_reg(ss.adr))
		trace(TRACE1, "\t  R%o=%03o", ss.adr, b_read(ss.adr, in_reg(ss.adr)));
	else
		trace(TRACE1, "[%06o]=%03o", ss.adr, b_read(ss.adr, in_reg(ss.adr)));

	if(dd.adr == odata){					//output
		if(current_log_level == INFO){
			if (!isatty(fileno(stdout)))
				fprintf(stdout, "%c", b_read(dd.adr, in_reg(dd.adr)));
			else
				fprintf(stderr, "%c", b_read(dd.adr, in_reg(dd.adr)));
		}
		else{
			trace(TRACE1, " [%06o] ", dd.adr);
			if (!isatty(fileno(stdout)))
				fprintf(stdout, "%c", b_read(dd.adr, in_reg(dd.adr)));
			else
				trace(TRACE1, "%c", b_read(dd.adr, in_reg(dd.adr)));
		}
		//w_write(odata, 0000000, in_reg(odata));
	}

	trace(TRACE1, "\n");
	trace2();
}
void do_add() {
	w_write(dd.adr, ss.val + dd.val, in_reg(dd.adr));
	PSW = psw(ss.val, dd.val);
	
	if(in_reg(ss.adr))
		trace(TRACE1, " \t  R%o=%06o", ss.adr, w_read(ss.adr, in_reg(ss.adr)));
	else
		trace(TRACE1, "[%06o]=%06o", ss.adr, w_read(ss.adr, in_reg(ss.adr)));

	if(in_reg(dd.adr))
		trace(TRACE1, " R%o=%06o", dd.adr, w_read(dd.adr, in_reg(dd.adr)));
	else
		trace(TRACE1, " [%06o]=%06o", dd.adr, w_read(dd.adr, in_reg(dd.adr)));

	trace(TRACE1, "\n");
	trace2();
}
void do_sub(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_sob() {
	if (--reg[r.adr] != 0){
		pc = pc - 2 * nn.val;
		trace(TRACE1, "%06o \n", r.adr, pc);
	}
	else
		trace(TRACE1, "%06o \n", r.adr, pc - 2 * nn.val);
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
void do_incb(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_clr() {
	reg[dd.adr] = 0;
	PSW = psw(reg[dd.adr], 0);
	trace(TRACE1, "\n");
	trace2();
}
void do_clrb(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_halt() {
	trace(TRACE1, "THE_END\n");
	trace2();
	fprintf(stderr, "\n---------------- halted --------------\n");
	fprintf(stderr, "R0=%06o R2=%06o R4=%06o SP=%06o\n", reg[0],reg[2],reg[4],reg[6]);
	fprintf(stderr, "R1=%06o R3=%06o R5=%06o PC=%06o\n", reg[1],reg[3],reg[5],reg[7]);
	char * nzvc = NZVC();
	fprintf(stderr, "psw=%06o: cm=k pm=k pri=0   %s [%d]\n", PSW, nzvc, counter);
	free(nzvc);
	exit(0);
}

void do_tst(){
	PSW = psw(dd.val, 0);
	if (in_reg(dd.adr))
		trace(TRACE1, " \t \tR%o=%06o\n", dd.adr, dd.val);
	else
		trace(TRACE1, "[%06o]=%06o\n", dd.adr, dd.val);
	trace2();

}
void do_tstb(){
	PSW = psw(dd.val >> 7 ? dd.val | 0xff00 : dd.val, 0);
	if (in_reg(dd.adr))
		trace(TRACE1, "R%o=%03o\n", dd.adr, dd.val);
	else
		trace(TRACE1, "[%06o]=%03o\n", dd.adr, dd.val);
	trace2();
}

void do_rol(){
	PSW = psw(dd.val, dd.val);
	dd.val = (dd.val << 1) & 0xfffe;
	w_write(dd.adr, dd.val, in_reg(dd.adr));
	if (in_reg(dd.adr))
		trace(TRACE1, " \t \tR%o=%06o\n", dd.adr, dd.val);
	else
		trace(TRACE1, "[%06o]=%06o\n", dd.adr, dd.val);
	trace2();
}
void do_rolb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_ror(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_rorb(){
	trace(TRACE1, "**the command has not been built**\n");
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

void do_com(){
	trace(TRACE1, "**the command has not been built**\n");
/*	dd.adr = ~ dd.adr;
	PSW = psw(dd.adr, 0) | 1;
	trace2();*/
}
void do_comb(){
	trace(TRACE1, "**the command has not been built**\n");
//	dd.adr = ~ dd.adr;
}

void do_cmp(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_cmpb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bic(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bicb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bis(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bisb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bit(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bitb(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_mul(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_div(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_ash(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_ashc(){
/*	if(ss.val < 0)
		w_write(r.adr, (ss.val >> 1) | (ss.val & 0x8000), in_reg(r.adr));
	else
		w_write(r.adr, ss.void << 1, in_reg(r.adr));
	PSW()*/

	trace(TRACE1, "**the command has not been built**\n");
}
void do_xor(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_adc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_adcb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_asl(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_aslb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_asr(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_asrb(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_dec(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_decb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_neg(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_negb(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sbc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sbcs(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_swab(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sxt(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_mtps(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_mfpi(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_mfpd(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_mtpi(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_mtpd(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_br(){
	//pc = (pc + 2 * xx) & 0xffff;
	pc = pc + 2 * xx;
	trace2();
}
void do_beq(){
	if (flag_Z())	//if z = 1
		do_br();
	else
		trace2();
}
void do_bne(){			//if z = 0
	if(!flag_Z())
		do_br();
	else
		trace2();
}
void do_bmi(){
	if (flag_N())	//if n = 1
		do_br();
	else
		trace2();
}
void do_bpl(){	
	if (!flag_N())	//if n = 0
		do_br();
	else
		trace2();
}
void do_blt(){
	if (flag_N() ^ flag_V())	//if n ^ v = 1
		do_br();
	else
		trace2();
}
void do_bge(){
	if (!(flag_N() ^ flag_V()))	//if n ^ v = 0
		do_br();
	else
		trace2();
}
void do_ble(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bcc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bcs(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bgt(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bhi(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bhis(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_blo(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_blos(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bvc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_bvs(){
	trace(TRACE1, "**the command has not been built**\n");
}

void do_ccc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_clc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_cln(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_clv(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_clz(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_scc(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sec(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sen(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sev(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_sez(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_nop(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_wait(){
	trace(TRACE1, "**the command has not been built**\n");
}
void do_reset(){
	trace(TRACE1, "**the command has not been built**\n");
}

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

char * NZVC () {
	char * res = malloc(5 * sizeof(char));
	res[4] = '\0';
	int cl = current_log_level;
	char c = cl == TRACE2 ? '-' : ' ';

	if (flag_C())
		res[3] = 'C';
	else
		res[3] = c;

	if (flag_V())
		res[2] = 'V';
	else
		res[2] = c;

	if (flag_Z())
		res[1] = 'Z';
	else
		res[1] = c;

	if (flag_N())
		res[0] = 'N';
	else
		res[0] = c; 

	return res;
}
int flag_N(){
	return (PSW & 8) >> 3;
}
int flag_Z(){
	return (PSW & 4) >> 2;
}
int flag_V(){
	return (PSW & 2) >> 1;
}
int flag_C(){
	return (PSW & 1);
}

void trace2(){
	char * nzvc = NZVC();
	trace(TRACE2, "%s 0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o S:%06o P:%06o\n", nzvc, reg[0],reg[1],reg[2],reg[3],reg[4],reg[5],reg[6],reg[7]);
	free(nzvc);
	//printf("psw=%06o: [%d]\n", PSW, counter);
}





