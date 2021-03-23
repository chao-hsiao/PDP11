//#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned char byte;			//8 bit
typedef unsigned short int word;	//16 bit
typedef word adr;					//16 bit

#define MEMSIZE (64 * 1024)

word mem[MEMSIZE];

void test_mem();

byte b_read(adr a);        		// читает из "старой памяти" mem байт с "адресом" a.
void b_write(adr a, byte val); 	// пишет значение val в "старую память" mem в байт с "адресом" a.
word w_read(adr a);            	// читает из "старой памяти" mem слово с "адресом" a.
void w_write(adr a, word val);  // пишет значение val в "старую память" mem в слово с "адресом" a.

void load_file(const char * filename);
void mem_dump(adr start, word n);


int main(int argc, char const *argv[]) {
    //test_mem();
    load_file(argv[1]);
    mem_dump(mem[0], mem[2]);

	return 0;
}

byte b_read(adr a) {
	if(a % 2 == 0)
    	return (byte)(mem[a] & 255);
    else
    	return (byte)(mem[a-1] >> 8 & 255);
}
void b_write(adr a, byte val) {
	if(a % 2 == 0)
    	mem[a] = val;
    else
    	mem[a-1] = ((word)val << 8 & ~255) | mem[a-1];
}
word w_read(adr a) {
    return mem[a];
}
void w_write(adr a, word val) {
	mem[a] = val;
}

void load_file(const char * filename) {
	byte x;
	word adr, n;
	FILE *fin;

	fin = fopen(filename, "r");
	fscanf(fin, "%hx", &adr);
	w_write(0, adr);
	//printf("%hd\n", adr);
	fscanf(fin, "%hx", &n);
	w_write(2, n);
	//printf("%hd\n", n);

	for (int i = 4; i < n + 4; ++i)
	{
		fscanf(fin, "%hhx", &x);
	    b_write(i, x);
	}

	fclose(fin);

}

void mem_dump(adr start, word n) {
	for (int i = 0; i < n/2; i++)
	{
		printf("%06o : %06o\n", start, w_read(4 + 2 * i));
		start = start + 2;
	}

}

void test_mem() {
/*
    //------------ when the mem is word
	word w0 = 0x0b0a;
	//пишим слов, читаем слов
	w_write(2, w0);
	word wres = w_read(2);
	printf("%04hx = %04hx\n", w0, wres);
	assert(w0 == wres);

	//пишим слов, читаем 2 байта
	adr a = 4;
	byte b1 = 0x0b;
	byte b0 = 0x0a;
	word w = 0x0b0a;
	w_write(a, w);
	word b0res = b_read(a);
	word b1res = b_read(a+1);

	printf("ww/br \t %04hx=%02hhx%02hhx\n", w, b1res, b0res);
	assert(b0 == b0res);  //0x0a
	assert(b1 == b1res);  //0x0b
    //-------------
*/

	
	//------------ when the mem is byte
	byte b0 = 0x0a;
	//пишим байт, читаем байт
	b_write(2, b0);
	byte bres = b_read(2);
	printf("%hhx = %hhx\n", b0, bres);
	assert(b0 == bres);

	//пишим 2 байта, читаем слов
	adr a = 4;
	byte b1 = 0x0b;
	b0 = 0x0a;
	word w = 0x0b0a;
	b_write(a, b0);
	b_write(a+1, b1);
	word wres = w_read(a);
	printf("ww/br \t %04hx=%02hhx%02hhx\n", wres, b1, b0);
	assert(wres == w);
    //-------------
	
}




