#ifndef __ALLOCATOR_H_
#define __ALLOCATOR_H_
#include "assert.h"
#include "color.h"
#include "types.h"
#include <stdarg.h>
#include <stddef.h>


extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

extern uint32_t alloc_start;
extern uint32_t alloc_end;

/* printf */
extern int printf(const char* s, ...);
extern void panic(char* s);

#define FREE (0)
#define ALLOCATED (1)
#define NIL (0)
#define OEDER 6

/* Interface */
void heap_init();
void* malloc(uint32_t size);
void free(void* ptr);

/* Tools */
uint32_t _align_up(uint32_t x, uint32_t n);
uint32_t _align_down(uint32_t x, uint32_t n);
/*
 * Heap boundary:
 *  1. prologue and epilogue is the boundary block of the heap
 *  2. has no payload, marked as allocated
 *  3. cannot be dynamically allocated/free
 */
uint32_t get_prologue();
uint32_t get_epilogue();
/* first and last block could be allocated */
uint32_t get_firstblock();
uint32_t get_lastblock();

/*
 * Block operations:
 * 	1. boundary_addr: header_addr or footer_addr
 * 	2. hp_addr: header_addr or payload_addr
 */
uint32_t get_blocksize(uint32_t boundary_addr);
void set_blocksize(uint32_t boundary_addr, uint32_t blocksize);
uint32_t get_allocated(uint32_t boundary_addr);
void set_allocated(uint32_t boundary_addr, uint32_t allocated);
uint32_t get_payload(uint32_t hp_addr);
uint32_t get_header(uint32_t hp_addr);
uint32_t get_footer(uint32_t hp_addr);

/* Heap operations */
uint32_t get_nextheader(uint32_t ph_addr);
uint32_t get_prevheader(uint32_t ph_addr);

#endif /* __ALLOCATOR_H_ */
