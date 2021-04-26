#include "pdp11.h"
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//enum LOGLEVEL current_log_level = INFO;
int current_log_level = INFO;

void run();
Arg get_modereg(word w);
Arg get_addressR(word);
Arg get_number(word);
Arg get_b(word);
Arg get_xx(word);

int main(int argc, char const *argv[]) {

	if(usage(argv, argc))
		return 1;
	load_file(argv, argc - 1);
	//mem_dump(0x200,0xc);

	printf("\n---------------- running --------------\n");

	run();

	return 0;
}

void run() {
	pc = 01000;

	while(1) {
		word w = w_read(pc, in_reg(pc));
		trace(TRACE1, "%06o %06o: ", pc, w);
		pc = pc + 2;
		counter++;

		for(int i = 0; ;i++) {
			cmd = command[i];
			if ((w & cmd.mask) == cmd.opcode) {
				trace(TRACE1, "%s\t", cmd.name);

				if(cmd.params == NO_PARAMS) {}
				if((cmd.params & HAS_B) == HAS_B)
					b = get_b(w >> 15);
				if((cmd.params & HAS_SS) == HAS_SS) {
					ss = get_modereg(w >> 6);
					trace(TRACE1, ",");
				}
				if((cmd.params & HAS_DD) == HAS_DD)
					dd = get_modereg(w);
				if((cmd.params & HAS_R) == HAS_R) {
					r = get_addressR((w & 0777) >> 6);
					trace(TRACE1, "R%o,", r.adr);
				}
				if((cmd.params & HAS_NN) == HAS_NN) {
					nn = get_number(w & 077);
					trace(DEBUG,"%o", nn.val);
				}
				if((cmd.params & HAS_N) == HAS_N) {
					n = get_number(w);
				}
				if((cmd.params & HAS_TT) == HAS_TT) {
					tt = get_number(w);
				}
				if((cmd.params & HAS_XX) == HAS_XX) {
					xx = get_xx(w);
				}
				
				b.val = 0;

				cmd.do_func();
				
				break;
			}
		}
	}
}

Arg get_modereg(word w) {
	Arg res;
	word regi, mode, index;
	trace(DEBUG,"%o ", w);
	regi = w & 07;
	trace(DEBUG,"%o ", regi);
	mode = w >> 3 & 07;
	trace(DEBUG,"%o ", mode);

	switch (mode) {
		case 0:				//Rn
			res.adr = regi;
			res.val = reg[regi];
			if (regi == 7)
				trace(TRACE1, "pc");
			else
				trace(TRACE1, "R%o", regi);
			break;
		case 1:				//(Rn)
			res.adr = reg[regi];
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "(pc)", regi);
			else
				trace(TRACE1, "(R%o)", regi);
			break;
		case 2:				//(Rn)+; if n = 7 -> #val
			res.adr = reg[regi];
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7){
				reg[regi] = reg[regi] + 2;
				trace(TRACE1, "#%06o", res.val);
			}
			else{
				reg[regi] = reg[regi] + (b.val ? 1 : 2);
				trace(TRACE1, "(R%o)+", regi);
			}
			break;
		case 3:				//@(Rn)+; if n = 7 -> @#adr
			res.adr = b.val ? b_read(reg[regi], in_reg(reg[regi])) : w_read(reg[regi], in_reg(reg[regi]));
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7){
				reg[regi] = reg[regi] + 2;
				trace(TRACE1, "@#%06o", res.adr);
			}
			else{
				reg[regi] = reg[regi] + (b.val ? 1 : 2);
				trace(TRACE1, "@(R%o)+", regi);
			}
			break;
		case 4:				//-(Rn)
			if (regi == 7)
				reg[regi] = reg[regi] - 2;
			else
				reg[regi] = reg[regi] - (b.val ? 1 : 2);
			res.adr = reg[regi];
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));	
			if (regi == 7)
				trace(TRACE1, "-(pc)");
			else
				trace(TRACE1, "-(R%o)", regi);
			break;
		case 5:				//@-(Rn)
			if (regi == 7)
				reg[regi] = reg[regi] - 2;
			else
				reg[regi] = reg[regi] - (b.val ? 1 : 2);
			res.adr = b.val ? b_read(reg[regi], in_reg(reg[regi])) : w_read(reg[regi], in_reg(reg[regi]));
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "@-(pc)");
			else
				trace(TRACE1, "@-(R%o)", regi);
			break;
		case 6:				//X(Rn); if n = 7 -> adr
			index = w_read(pc, in_reg(pc));
			pc = pc + 2;
			res.adr = reg[regi] + index;
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "%06o ", res.adr);
			else
				trace(TRACE1, "%o(R%o)", index, regi);
			break;
		case 7:				//@X(Rn); if n = 7 -> @adr
			index = w_read(pc, in_reg(pc));
			pc = pc + 2;
			res.adr = b.val ? b_read(reg[regi] + index, in_reg(reg[regi] + index)) : w_read(reg[regi] + index, in_reg(reg[regi] + index));
			res.val = b.val ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "@%06o", res.adr);
			else
				trace(TRACE1, "@%o(R%o)", index, regi);
			break;
		default:
			fprintf(stderr, "Mode %o is not created yet\n", mode);
			exit(1);
	}

	return res;
}

Arg get_b(word w){
	Arg res;
	res.val = w;
	return res;
}
Arg get_number(word w){
	Arg res;
	res.val = w;
	return res;
}
Arg get_addressR(word w){
	Arg res;
	res.adr = w;
	return res;
}
Arg get_xx(word w){}

void trace(int log_level, const char * format, ...){
	// никакой печати, если логирование ниже по уровню
	if (log_level < current_log_level)
		return;
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}
