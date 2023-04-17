#ifndef __TASK_COMMON_H__
#define __TASK_COMMON_H__

#include "assert.h"
#include "types.h"
#include "ulib.h"

// #define DELAY 500

/* printf */
extern int printf(const char* s, ...);             // only understands %d, %x, %p, %s
extern void print_memory(void* ptr, size_t size);  // print out memory data in bit

/* string tools */
// extern void delay(volatile int count);

#endif /* __TASK_COMMON_H__ */

