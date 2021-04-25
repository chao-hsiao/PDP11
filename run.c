#include "pdp11.h"
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//enum LOGLEVEL current_log_level = INFO;
int current_log_level = INFO;

void run();
Arg get_modereg(word w, int is_byte);

int main(int argc, char const *argv[]) {
	//printf("%d\n", argc);

	if (argc <= 1) {
		usage(argv[0]);
		return 1;
	}
	if (argc == 3 && strcmp("-t", argv[2]))
		current_log_level = TRACE;

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
		trace(TRACE, "%06o %06o: ", pc, w);
		pc = pc + 2;

		for(int i = 0; ;i++) {
			cmd = command[i];
			if ((w & cmd.mask) == cmd.opcode) {
				trace(TRACE, "%s\t", cmd.name);

				if(cmd.params == NO_PARAMS) {}

				if((cmd.params & HAS_SS) == HAS_SS) {
					ss = get_modereg(w >> 6, w >> 15);
					trace(TRACE, ",");
				}
				if((cmd.params & HAS_DD) == HAS_DD)
					dd = get_modereg(w, w >> 15);
				if((cmd.params & HAS_R) == HAS_R) {
					rnn.adr = (w & 0777) >> 6;
					trace(TRACE, "R%o,", rnn.adr);
				}
				if((cmd.params & HAS_NN) == HAS_NN) {
					rnn.val = w & 077;
					trace(DEBUG,"%o", rnn.val);
				}
				if((cmd.params & HAS_XX) == HAS_XX) {}
				if((cmd.params & HAS_N) == HAS_N) {}
				if((cmd.params & HAS_TT) == HAS_TT) {}
				if((cmd.params & HAS_B) == HAS_B) {}

				cmd.do_func();
				
				break;
			}
		}
	}
}

Arg get_modereg(word w, int is_byte) {
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
				trace(TRACE, "pc");
			else
				trace(TRACE, "R%o", regi);
			break;
		case 1:				//(Rn)
			res.adr = reg[regi];
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE, "(pc)", regi);
			else
				trace(TRACE, "(R%o)", regi);
			break;
		case 2:				//(Rn)+; if n = 7 -> #val
			res.adr = reg[regi];
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			reg[regi] = reg[regi] + (is_byte ? 1 : 2);
			if (regi == 7)
				trace(TRACE, "#%06o", res.val);
			else
				trace(TRACE, "(R%o)+", regi);
			break;
		case 3:				//@(Rn)+; if n = 7 -> @#adr
			res.adr = is_byte ? b_read(reg[regi], in_reg(reg[regi])) : w_read(reg[regi], in_reg(reg[regi]));
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			reg[regi] = reg[regi] + (is_byte ? 1 : 2);
			if (regi == 7)
				trace(TRACE, "@#%06o", res.adr);
			else
				trace(TRACE, "@(R%o)+", regi);
			break;
		case 4:				//-(Rn)
			reg[regi] = reg[regi] - 2;
			res.adr = reg[regi];
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));	
			if (regi == 7)
				trace(TRACE, "-(pc)");
			else
				trace(TRACE, "-(R%o)", regi);
			break;
		case 5:				//@-(Rn)
			reg[regi] = reg[regi] - (is_byte ? 1 : 2);
			res.adr = is_byte ? b_read(reg[regi], in_reg(reg[regi])) : w_read(reg[regi], in_reg(reg[regi]));
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE, "@-(pc)");
			else
				trace(TRACE, "@-(R%o)", regi);
			break;
		case 6:				//X(Rn); if n = 7 -> adr
			index = w_read(pc, in_reg(pc));
			pc = pc + 2;
			res.adr = reg[regi] + index;
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE, "%06o ", res.adr);
			else
				trace(TRACE, "%o(R%o)", index, regi);
			break;
		case 7:				//@X(Rn); if n = 7 -> @adr
			index = w_read(pc, in_reg(pc));
			pc = pc + 2;
			res.adr = is_byte ? b_read(reg[regi] + index, in_reg(reg[regi] + index)) : w_read(reg[regi] + index, in_reg(reg[regi] + index));
			res.val = is_byte ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE, "@%06o", res.adr);
			else
				trace(TRACE, "@%o(R%o)", index, regi);
			break;
		default:
			fprintf(stderr, "Mode %o is not created yet\n", mode);
			exit(1);
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
