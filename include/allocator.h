#ifndef __ALLOCATOR_H_
#define __ALLOCATOR_H_
#include "assert.h"
#include "types.h"
#include "color.h"
#include <stddef.h>
#include <stdarg.h>

extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

extern uint32_t alloc_start;
extern uint32_t alloc_end;

/* printf */
extern int  printf(const char* s, ...);
extern void panic(char *s);

#define FREE (0)
#define ALLOCATED (1)
#define NIL (0)
#define OEDER 6

/* interface */
void heap_init();
void *malloc(uint32_t size);
void free(void *ptr);

/* tools */
uint32_t _align_up(uint32_t x, uint32_t n);
uint32_t _align_down(uint32_t x, uint32_t n);
extern uint32_t get_prologue();
extern uint32_t get_epilogue();

/* block operations */
uint32_t get_blocksize(uint32_t header_addr);
void set_blocksize(uint32_t header_addr, uint32_t blocksize);
uint32_t get_allocated(uint32_t header_addr);
void set_allocated(uint32_t header_addr, uint32_t allocated);
uint32_t get_payload(uint32_t addr);
uint32_t get_header(uint32_t addr);
uint32_t get_footer(uint32_t addr);

/* heap operations */
uint32_t get_nextheader(uint32_t addr);
uint32_t get_prevheader(uint32_t addr);
uint32_t get_firstblock();
uint32_t get_lastblock();

#endif /* __ALLOCATOR_H_ */
