#include "common.h"

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR   0x002
#define O_CREATE 0x200
#define O_TRUNC  0x400

/* C std */
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

/* Hardware physical write/read handler */

// macro for quick execute more quick...
#ifndef SIMULATE_QEMU
#define READ_BURST_64(base, offset)         (*(volatile uint64_t*)((base) + (offset)))
#define WRITE_BURST_64(base, offset, value) (*(volatile uint64_t*)((base) + (offset)) = (value))
#define READ_BURST_32(base, offset)         (*(volatile uint32_t*)((base) + (offset)))
#define WRITE_BURST_32(base, offset, value) (*(volatile uint32_t*)((base) + (offset)) = (value))
#define READ_BURST_16(base, offset)         (*(volatile uint16_t*)((base) + (offset)))
#define WRITE_BURST_16(base, offset, value) (*(volatile uint16_t*)((base) + (offset)) = (value))
#else
#define READ_BURST_64(base, offset) \
  printf("READ_BURST_64: Reading address %p\n", (base) + (offset));
#define WRITE_BURST_64(base, offset, value) \
  printf("WRITE_BURST_64: Writing %p to address %p\n", (base) + (offset), (value));
#define READ_BURST_32(base, offset) \
  printf("READ_BURST_32: Reading address %p\n", (base) + (offset), (value));
#define WRITE_BURST_32(base, offset, value) \
  printf("WRITE_BURST_32: Writing %p to address %p\n", (base) + (offset));
#define READ_BURST_16(base, offset) \
  printf("READ_BURST_16: Reading address %p\n", (base) + (offset));
#define WRITE_BURST_16(base, offset, value) \
  printf("WRITE_BURST_16: Writing %p to address %p\n", (base) + (offset), (value));
#endif

