#ifndef PDP11_H
#define PDP11_H


typedef unsigned char byte;			//8 bit
typedef unsigned short int word;	//16 bit
typedef word adr;					//16 bit

#define MEMSIZE (64 * 1024)
#define REGSIZE 8

extern word mem[MEMSIZE];
extern word reg[REGSIZE];

/*
enum LOGLEVEL {
	DEBUG = 1,	// debug messages
	TRACE = 2,	// trace
	INFO = 3,
	ERROR = 4
};
*/
#define DEBUG 1
#define TRACE 2
#define INFO 3
#define ERROR 4

void test_mem();

byte b_read(adr a);        		// читает из "старой памяти" mem байт с "адресом" a.
void b_write(adr a, byte val); 	// пишет значение val в "старую память" mem в байт с "адресом" a.
word w_read(adr a);            	// читает из "старой памяти" mem слово с "адресом" a.
void w_write(adr a, word val);  // пишет значение val в "старую память" mem в слово с "адресом" a.

void load_file(const char ** filename);
void mem_dump(adr start, word n);

// void trace(enum LOGLEVEL log_level, const char * c, ...);
void trace(int log_level, const char * c, ...);

#endif
