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
Arg get_address(word w);
Arg get_number(word w);
int b;
char xx;

int main(int argc, char const *argv[]) {

	if(usage(argv, argc))
		return 1;
	load_file(argv[argc - 1]);
	//mem_dump(0x200,0xe);

	fprintf(stderr, "\n---------------- running --------------\n");

	run();

	return 0;
}

void run() {
	w_write(ostat, 0000200, in_reg(ostat));
	//w_write(odata, 0000000, in_reg(odata));
	//w_write(0177776, 0014347, in_reg(0177776));
	w_write(0177774, 0000000, in_reg(0177774));
	//w_write(0177560, , in_reg(0177560));
	//w_write(0177562, , in_reg(0177562));

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

				b = has_byte();
				if(cmd.params == NO_PARAMS) {}
				//if(cmd.params & HAS_B)	
				if((cmd.params & HAS_R) == HAS_R) {
					if(cmd.opcode == 0000200)
						r = get_address(w & 7);
					else
						r = get_address((w >> 6) & 7);
					if(r.adr == 7)
						trace(TRACE1, "PC");
					else if(r.adr == 6)
						trace(TRACE1, "SP");
					else
						trace(TRACE1, "R%o", r.adr);
					if(cmd.opcode != 0000200)
						trace(TRACE1, ",");
				}
				if((cmd.params & HAS_SS) == HAS_SS) {
					ss = get_modereg(w >> 6);
					if(cmd.mask == 0170000)
						trace(TRACE1, ",");
					else
						trace(TRACE1, " \t  ");
				}
				if((cmd.params & HAS_DD) == HAS_DD){
					dd = get_modereg(w);
					trace(TRACE1, " \t  ");
				}
				if((cmd.params & HAS_NN) == HAS_NN) {
					nn = get_number(w & 077);
					trace(DEBUG1,"nn.val=%o", nn.val);
				}
				if((cmd.params & HAS_N) == HAS_N) {
					n = get_number(w);
				}
				if((cmd.params & HAS_TT) == HAS_TT) {
					tt = get_number(w);
				}
				if((cmd.params & HAS_XX) == HAS_XX) {
					xx = get_xx(w);
					trace(TRACE1, "%06o\n", pc + 2 * xx);
				}

				cmd.do_func();
				
				break;
			}
		}
	}
}

Arg get_modereg(word w) {
	Arg res;
	word regi, mode, index;
	trace(DEBUG1,"w=%o ", w);
	regi = w & 07;
	trace(DEBUG1,"regi=%o ", regi);
	mode = w >> 3 & 07;
	trace(DEBUG1,"mode=%o ", mode);

	switch (mode) {
		case 0:				//Rn
			res.adr = regi;
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "PC");
			else if (regi == 6)
				trace(TRACE1, "SP");
			else
				trace(TRACE1, "R%o", regi);
			break;
		case 1:				//(Rn)
			res.adr = reg[regi];
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "(PC)");
			else if (regi == 6)
				trace(TRACE1, "(SP)");
			else
				trace(TRACE1, "(R%o)", regi);
			break;
		case 2:				//(Rn)+; if n = 7 -> #val
			res.adr = reg[regi];
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7){
				pc = pc + 2;
				trace(TRACE1, "#%06o", res.val);
			}
			else if (regi == 6){
				sp = sp + 2;
				trace(TRACE1, "(SP)+");
			}
			else{
				reg[regi] = reg[regi] + (b ? 1 : 2);
				trace(TRACE1, "(R%o)+", regi);
			}
			break;
		case 3:				//@(Rn)+; if n = 7 -> @#adr
			res.adr = w_read(reg[regi], in_reg(reg[regi]));
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			reg[regi] = reg[regi] + 2;
			if (regi == 7)
				trace(TRACE1, "@#%06o", res.adr);
			else if (regi == 6)
				trace(TRACE1, "@(SP)+");
			else
				trace(TRACE1, "@(R%o)+", regi);
			break;
		case 4:				//-(Rn)
			if (regi == 7 & regi == 6)
				reg[regi] = reg[regi] - 2;
			else
				reg[regi] = reg[regi] - (b ? 1 : 2);
			res.adr = reg[regi];
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));	
			if (regi == 7)
				trace(TRACE1, "-(PC)");
			else if (regi == 6)
				trace(TRACE1, "-(SP)");
			else
				trace(TRACE1, "-(R%o)", regi);
			break;

		case 5:				//@-(Rn)
			reg[regi] = reg[regi] - 2;
			res.adr = w_read(reg[regi], in_reg(reg[regi]));
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "@-(PC)");
			else if(regi == 6)
				trace(TRACE1, "@-(SP)");
			else
				trace(TRACE1, "@-(R%o)", regi);
			break;
		case 6:				//X(Rn); if n = 7 -> adr
			index = w_read(pc, in_reg(pc));
			pc = pc + 2;
			res.adr = reg[regi] + index;
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "%06o ", res.adr);
			else if (regi == 6)
				trace(TRACE1, "%06o(SP)", index);
			else
				trace(TRACE1, "%06o(R%o)", index, regi);
			break;
		case 7:				//@X(Rn); if n = 7 -> @adr
			index = w_read(pc, in_reg(pc));
			pc = pc + 2;
			res.adr = w_read(reg[regi] + index, in_reg(reg[regi] + index));
			res.val = b ? b_read(res.adr, in_reg(res.adr)) : w_read(res.adr, in_reg(res.adr));
			if (regi == 7)
				trace(TRACE1, "@%06o", res.adr);
			else if (regi == 6)
				trace(TRACE1, "@%06o(SP)", index);
			else
				trace(TRACE1, "@%06o(R%o)", index, regi);
			break;
		default:
			fprintf(stderr, "Mode %o is not created yet\n", mode);
			exit(1);
	}

	return res;
}

int has_byte(){
	return cmd.params & HAS_B;
}
char get_xx(word w){
	return w;
}
Arg get_number(word w){
	Arg res;
	res.val = w;
	return res;
}
Arg get_address(word w){
	Arg res;
	res.adr = w;
	return res;
}


void trace(int log_level, const char * format, ...){
	// никакой печати, если логирование ниже по уровню
	if (log_level < current_log_level)
		return;
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}



