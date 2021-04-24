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

int main(int argc, char const *argv[]) {
	printf("%d\n", argc);

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
		word w = w_read(pc);
		trace(TRACE, "%06o %06o: ", pc, w);
		pc = pc + 2;

		for(int i = 0; ;i++) {
			cmd = command[i];
			if ((w & cmd.mask) == cmd.opcode) {
				trace(TRACE, "%s ", cmd.name);
				if(cmd.params == NO_PARAMS) {}

				if((cmd.params & 2) == HAS_SS) {
					ss = get_modereg(w >> 6);
					trace(TRACE, ",");
				}
				if((cmd.params & 1) == HAS_DD)
					dd = get_modereg(w);
				if((cmd.params & 8) == HAS_R) {
					rnn.adr = (w & 0777) >> 6;
					trace(TRACE, "R%o,", rnn.adr);
				}
				if((cmd.params & 4) == HAS_NN) {
					rnn.val = w & 077;
					trace(TRACE,"%o", rnn.val);
				}
				if((cmd.params & 16) == HAS_XX) {
					
				}

				cmd.do_func();
				trace(TRACE, "\n");
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
