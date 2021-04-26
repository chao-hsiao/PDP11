#ifndef PDP11_H
#define PDP11_H


typedef unsigned char byte;			//8 bit
typedef unsigned short int word;	//16 bit
typedef word adr;					//16 bit

#define MEMSIZE (64 * 1024)
#define REGSIZE 8
#define pc reg[7]

#define NO_PARAMS 0
#define HAS_DD 1
#define HAS_SS 2
#define HAS_R 8
#define HAS_NN 4
#define HAS_N 32
#define HAS_XX 16
#define HAS_TT 64
#define HAS_B 128


extern word mem[MEMSIZE];
extern word reg[REGSIZE];
extern word PSW;
extern int counter;
extern int current_log_level;

/*
enum LOGLEVEL {
	DEBUG = 1,	// debug messages
	TRACE = 2,	// trace
	INFO = 3,
	ERROR = 4
};
*/
#define DEBUG 1
#define TRACE2 2
#define TRACE1 3
#define INFO 4
#define ERROR 5

void test_mem();

byte b_read(adr a, int in_reg);        		// читает из "старой памяти" mem байт с "адресом" a.
void b_write(adr a, byte val, int in_reg); 	// пишет значение val в "старую память" mem в байт с "адресом" a.
word w_read(adr a, int in_reg);            	// читает из "старой памяти" mem слово с "адресом" a.
void w_write(adr a, word val, int in_reg);  // пишет значение val в "старую память" mem в слово с "адресом" a.

void load_file(const char ** filename, int argc);
void mem_dump(adr start, word n);

int in_reg(adr a);

int usage(const char ** argv, int argc);

// void trace(enum LOGLEVEL log_level, const char * c, ...);
void trace(int log_level, const char * c, ...);

#endif
