#ifndef __COMMON_H__
#define __COMMON_H__

#include "assert.h"
#include "font.h"
#include "types.h"
#include "lli.h"
#include <stdarg.h>
#include <stddef.h>

#define DELAY 500
#define STACK_SIZE 1024

/* printf */
extern int printf(const char* s, ...);  // only understands %d, %x, %p, %s

/* heap memory management */
extern void* malloc(uint32_t size); // input the bit size
extern void free(void* ptr);
extern void delay(volatile int count);

/* dma tools */
extern lli_t* malloc_LLI(void);
extern void free_LLI(lli_t* ptr);

/* string tools */
extern char* strcpy(char*, const char*);
extern void* memmove(void*, const void*, int);
extern char* strchr(const char*, char c);
extern int strcmp(const char*, const char*);
extern uint32_t strlen(const char*);
extern void* memset(void*, int, uint32_t);
extern int atoi(const char*);
extern int memcmp(const void*, const void*, uint32_t);
extern void* memcpy(void*, const void*, uint32_t);
extern void delay(volatile int count);

#endif /* __COMMON_H__ */
