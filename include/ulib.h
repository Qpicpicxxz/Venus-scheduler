#include "os.h"

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200
#define O_TRUNC 0x400
char* strcpy(char*, const char*);
void* memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
uint32_t strlen(const char*);
void* memset(void*, int, uint32_t);
int atoi(const char*);
int memcmp(const void*, const void*, uint32_t);
void* memcpy(void*, const void*, uint32_t);
