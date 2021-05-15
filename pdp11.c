#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

word mem[MEMSIZE];	//RAM
word reg[REGSIZE];	//registers
word PSW;			//process state word(слово состояния процессор)
int counter = 0;
//int str;

//byte read
byte b_read(adr a, int in_reg) {
	if(in_reg)
		return reg[a];
	else
		if(a % 2 == 0)
    		return (byte)(mem[a] & 0xff);
    	else
    		return (byte)(mem[a-1] >> 8 & 0xff);
}
//byte write
void b_write(adr a, byte val, int in_reg) {
	if(in_reg)
		reg[a] = val >> 7 ? val | 0xff00 : val;
	else
		if(a % 2 == 0)
    		mem[a] = val;
    	else
    		mem[a-1] = ((word)val << 8 & ~255) | mem[a-1];
}
//word read
word w_read(adr a, int in_reg) {
	if(in_reg)
		return reg[a];
    return mem[a];
}
//word write
void w_write(adr a, word val, int in_reg) {
	if (in_reg)
		reg[a] = val;
	else
		mem[a] = val;
}

//to load file in the RAM
void load_file(const char * filename) {
	byte x;
	word adr, n;
	FILE *fin;

	fin = fopen(filename, "r");

	if (fin == NULL) {
		perror(filename);
		//fprintf(stderr, "%s: can't open %s for reading\n", filename[0], filename[argc]);
		exit(1);
	}

    while(2 == fscanf(fin, "%hx%hx", &adr, &n)) {

    	//fprintf(stderr, "%04hx\n", adr);
    	//fprintf(stderr, "%04hx\n", n);

    	for (int i = 0; i < n; ++i)
    	{
    		fscanf(fin, "%hhx", &x);
    		//fprintf(stderr, "%02hhx\n", x);
    	    b_write(adr + i, x, in_reg(adr + i));
    	}
    }

	fclose(fin);
}

//yes or no to put in reg
int in_reg(adr a) {
	return (0 <= a && a < 8);
}
//forget it
void mem_dump(adr start, word n) {
	for (int i = 0; i < n/2; i++)
	{
		fprintf(stderr, "%06o : %06o\n", start, w_read(start, in_reg(start)));
		start = start + 2;
	}

}
//to remember to input the file name
int usage(const char ** argv, int argc) {
	if (argc <= 1){
		//fprintf(stderr, "\nUSAGE: %s pdp_filename\n\n", argv[0]);
		fprintf(stderr, "\nUsage: %s [options] initial-core-file [options to emulated program]\n", argv[0]);
		fprintf(stderr, "\nOptions:\n");
		fprintf(stderr, "\t-t\tshow trace to stderr\n");
		fprintf(stderr, "\t-T\tshow verbose trace to stderr\n");
		return 1;
	}
	else if (argc == 3)
	{
		if(!strcmp("-t", argv[1])){
			current_log_level = TRACE1;
			return 0;
		}
		else if(!strcmp("-T", argv[1])){
			current_log_level = TRACE2;
			return 0;
		}
		else if(!strcmp("-d", argv[1])){
			current_log_level = DEBUG1;
			return 0;
		}
		fprintf(stderr, "%s: bad flag %s\n", argv[0], argv[1]);
		fprintf(stderr, "\nUsage: %s [options] initial-core-file [options to emulated program]\n", argv[0]);
		fprintf(stderr, "\nOptions:\n");
		fprintf(stderr, "\t-t\tshow trace to stderr\n");
		fprintf(stderr, "\t-T\tshow verbose trace to stderr\n");
		return 1;
	}
	return 0;
}


/*
void test_mem() {

    //------------ when the mem is word
	word w0 = 0x0b0a;
	//пишим слов, читаем слов
	w_write(2, w0);
	word wres = w_read(2);
	fprintf(stderr, "%04hx = %04hx\n", w0, wres);
	assert(w0 == wres);

	//пишим слов, читаем 2 байта
	adr a = 4;
	byte b1 = 0x0b;
	byte b0 = 0x0a;
	word w = 0x0b0a;
	w_write(a, w);
	word b0res = b_read(a);
	word b1res = b_read(a+1);

	fprintf(stderr, "ww/br \t %04hx=%02hhx%02hhx\n", w, b1res, b0res);
	assert(b0 == b0res);  //0x0a
	assert(b1 == b1res);  //0x0b
    //-------------
*/

	/*
	//------------ when the mem is byte
	byte b0 = 0x0a;
	//пишим байт, читаем байт
	b_write(2, b0);
	byte bres = b_read(2);
	fprintf(stderr, "%hhx = %hhx\n", b0, bres);
	assert(b0 == bres);

	//пишим 2 байта, читаем слов
	adr a = 4;
	byte b1 = 0x0b;
	b0 = 0x0a;
	word w = 0x0b0a;
	b_write(a, b0);
	b_write(a+1, b1);
	word wres = w_read(a);
	fprintf(stderr, "ww/br \t %04hx=%02hhx%02hhx\n", wres, b1, b0);
	assert(wres == w);
    //-------------
	
}


*/

