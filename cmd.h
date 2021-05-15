//#ifndef CMD_H
//#define CMD_H
#pragma once
#include "pdp11.h"

typedef struct {
	word mask;
	word opcode;
	char * name;
	char params;
	void (*do_func)(void);
} Command;
/*
struct {
    char N;
    char Z;
    char V;
    char C;
} psw;
*/
typedef struct {
	word val;
	word adr;
} Arg;

int has_byte();
extern int b;
char get_xx(word w);
extern char xx;
extern Arg ss, dd, nn, n, r, tt;
extern Command command[];
extern Command cmd;

word psw(word w0, word w1);
int flag_N();
int flag_Z();
int flag_V();
int flag_C();
char * NZVC ();
void trace2();

void do_mov();
void do_movb();
void do_add();
void do_sub();
void do_cmp();
void do_cmpb();
void do_bic();
void do_bicb();
void do_bis();
void do_bisb();
void do_bit();
void do_bitb();

void do_mul();
void do_div();
void do_ash();
void do_ashc();
void do_xor();
void do_jmp();
void do_rol();
void do_rolb();
void do_ror();
void do_rorb();
void do_com();
void do_comb();
void do_adc();
void do_adcb();
void do_asl();
void do_aslb();
void do_asr();
void do_asrb();
void do_clr();
void do_clrb();
void do_tst();
void do_tstb();
void do_dec();
void do_decb();
void do_inc();
void do_incb();
void do_neg();
void do_negb();
void do_sbc();
void do_sbcs();
void do_swab();
void do_sxt();
void do_mtps();
void do_mfpi();
void do_mfpd();
void do_mtpi();
void do_mtpd();

void do_br();
void do_beq();
void do_bne();
void do_bmi();
void do_bpl();
void do_blt();
void do_bge();
void do_ble();
void do_bcc();
void do_bcs();
void do_bgt();
void do_bhi();
void do_bhis();
void do_blo();
void do_blos();
void do_bvc();
void do_bvs();

void do_sob();

void do_jsr();

void do_rts();

void do_ccc();
void do_clc();
void do_cln();
void do_clv();
void do_clz();
void do_scc();
void do_sec();
void do_sen();
void do_sev();
void do_sez();
void do_nop();

void do_halt();
void do_wait();
void do_reset();
void do_nothing();

//#endif