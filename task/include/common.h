#ifndef __TASK_COMMON_H__
#define __TASK_COMMON_H__

#include "assert.h"
#include "ctrlregs.h"
#include "device.h"
#include "types.h"
#include "ulib.h"

#define DELAY 500

/* printf */
extern void uart_putc(char ch);
extern void uart_puts(char* s);
extern int printf(const char* s, ...);             // only understands %d, %x, %p, %s
extern void print_memory(void* ptr, size_t size);  // print out memory data in bit

/* tools */
extern void vcs_stop(void);  // an assenble to pause vcs
extern void delay(volatile int count);

/* heap memory management */
extern void* malloc(uint32_t size);  // input the bit size
extern void free(void* ptr);

/* block handlers */
extern void job_done_invoke(void);

#endif /* __TASK_COMMON_H__ */

