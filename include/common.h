#ifndef __COMMON_H__
#define __COMMON_H__

#include "assert.h"
#include "types.h"
#include "ulib.h"

#define DELAY 500

/* printf */
extern void uart_putc(char ch);
extern void uart_puts(char* s);
extern int printf(const char* s, ...);             // only understands %d, %x, %p, %s
extern void print_memory(void* ptr, size_t size);  // print out memory data in bit

/* start.S */
extern void vcs_stop(void);  // assemble codes for stop vcs simulation

/* heap memory management */
extern void* malloc(uint32_t size); // input the bit size
extern void free(void* ptr);

/* string tools */
extern void delay(volatile int count);

#endif /* __COMMON_H__ */

