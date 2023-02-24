#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"
#include "riscv.h"
#include "page.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);

/* printf */
extern int  printf(const char* s, ...);
extern void panic(char *s);

/* memory management */
// returns a void pointer to the allocated space, or NULL if there is insufficient memory available
extern void *malloc(int bit);	// input the bit size
extern void free(void *ptr);

/* task management */
struct context {
	/* ignore x0 */
	reg_t ra;
	reg_t sp;	// stack pointer
	reg_t gp;	
	reg_t tp;	
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;	// frame pointer
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	
	// sx: callee-saved
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;
};

extern int  task_create(void (*task)(void));
extern void task_delay(volatile int count);
extern void task_yield();

#endif /* __OS_H__ */
