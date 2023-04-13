#ifndef __ULIB_H_
#define __ULIB_H_

#include "common.h"

#define READ_BURST_64(base, offset)         (*(volatile uint64_t*)((base) + (offset)))
#define WRITE_BURST_64(base, offset, value) (*(volatile uint64_t*)((base) + (offset)) = (value))
#define READ_BURST_32(base, offset)         (*(volatile uint32_t*)((base) + (offset)))
#define WRITE_BURST_32(base, offset, value) (*(volatile uint32_t*)((base) + (offset)) = (value))
#define READ_BURST_16(base, offset)         (*(volatile uint16_t*)((base) + (offset)))
#define WRITE_BURST_16(base, offset, value) (*(volatile uint16_t*)((base) + (offset)) = (value))

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200
#define O_TRUNC 0x400
#define DELAY 500

char* strcpy(char*, const char*);
void* memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
uint32_t strlen(const char*);
void* memset(void*, int, uint32_t);
int atoi(const char*);
int memcmp(const void*, const void*, uint32_t);
void* memcpy(void*, const void*, uint32_t);
void delay(volatile int count);

#endif  /* __ULIB_H_ */

