// prevents a header file from being included multiple times in the same program
#ifndef __PAGE_H
#define __PAGE_H

#include "os.h"

extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/*
 _alloc_start --- the actual start address of heap pool
 _alloc_end --- the actual end address of heap pool
 _num_pages --- the actual max number of pages we can allocate.
*/
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

/* the minimum memory allocate unit
   so if we call malloc(), it would at least allocate 256bits
   if want to adjust, you should modify three macro below
   the minimum metadata index size is RAM_SIZE / PAGE_SIZE^2
*/
#define PAGE_SIZE 256  // 256 Byte
#define PAGE_ORDER 8   // 256 = 2^8
#define META_SIZE 2048 // 0x0800_0000 / (256 * 256)

/* 
  a preprocessor directive which defines a constant macro,
  (1 << 2) is bit shift operation,
  shifts the binary representation of the integer 1
  two places to the left, which represent the value of 0b00000100
  pageNum implement round up to an integer
*/
#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST (uint8_t)(1 << 1)
#define pageNum(x) ((x - 1) / (PAGE_SIZE)) + 1

/*
  Page descriptor flags:
  1. flags = 0 --- this page is allocated
  2. flags = 1 --- this page is the last page of the memory block allocated
*/
struct Page {
  uint8_t flags;
};

static inline void _clear(struct Page *page) { page->flags = 0; }

// get page index pointer
static inline struct Page *_get_page(void *p) {
  struct Page *page = (struct Page *)HEAP_START;
  page += ((uint32_t)p - _alloc_start) / PAGE_SIZE;
  return page;
}

// get alloc page pointer
static inline void *_get_addr(struct Page *page) {
  /*
    1. page - (sturct *Page)HEAP_START 
       calculates the offset of the given page from the beginning of the heap
    2. each page is 32 `page` structures long
    3. (num * PAGE_SIZE) to get the starting address of the page within the heap
  */
  uint32_t num = (uint32_t)(page - (struct Page *)HEAP_START) / 32;
  return (void *)((uint32_t)_alloc_start + num * PAGE_SIZE);
}

static inline int _is_free(struct Page *page) {
  if (page->flags & PAGE_TAKEN) {
    return 0;
  } else {
    return 1;
  }
}

// make the bit specified by m_flags to 1
static inline void _set_flag(struct Page *page, uint8_t m_flags) {
  page->flags |= m_flags;
}

static inline int _is_last(struct Page *page) {
  if (page->flags & PAGE_LAST) {
    return 1;
  } else {
    return 0;
  }
}

/*
  align the address to the nearest page boundary (4KiB)
  to ensure that all operations are four-byte aligned
*/
static inline uint32_t _align_page(uint32_t address) {
  /*
    order is equivalent to `4096 -1`
    0000_0000_0000_0000_0001_0000_0000_0000 - 1 =
    0000_0000_0000_0000_0000_1111_1111_1111
  */
  uint32_t order = (1 << PAGE_ORDER) - 1;
  // set the lowest 12bits to 0 and leaving the higher bits unchanged
  // make the input address rounded up to the nearest multiple of the page size
  return (address + order) & (~order);
}

#endif

