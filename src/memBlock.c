#include "allocator.h"
#include "types.h"

extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/* 8 Byte alignment */

/* Round to multiple of n */
uint32_t _align_up(uint32_t x, uint32_t n) { return n * ((x + n - 1) / n); }
uint32_t _align_down(uint32_t x, uint32_t n) { return n * (x / n); }

inline uint32_t get_prologue() { return alloc_start + 4; }
inline uint32_t get_epilogue() { return alloc_end - 4; }

/* Block operations */
#define AF_BIT (0)
#define P8_BIT (1)
#define B8_BIT (2)

/* applicapable for both header && footer */
uint32_t get_blocksize(uint32_t header_addr) {
  assert(get_prologue() <= header_addr && header_addr <= get_epilogue());
  assert((header_addr & 0x3) == 0x0);
  if (header_addr == NIL) {
    return 0;
  } else {
    return (*(uint32_t *)header_addr & 0xFFFFFFF8);
  }
}

void set_blocksize(uint32_t header_addr, uint32_t blocksize) {
  assert((header_addr & 0x03) == 0x0); // 4 byte aligned
  assert((blocksize & 0x7) == 0x0);    // 8 byte aligned
  *(uint32_t *)header_addr &= 0x7;
  *(uint32_t *)header_addr |= blocksize;
}

/* applicapable for both header & footer for ordinary blocks */
uint32_t get_allocated(uint32_t header_addr) {
  if (header_addr == NIL) {
    /* NULL could be considered as allocated */
    return ALLOCATED;
  }
  assert(get_prologue() <= header_addr && header_addr <= get_epilogue());
  assert((header_addr & 0x3) == 0x0);
  return (*(uint32_t *)header_addr & 0x1);
}

void set_allocated(uint32_t header_addr, uint32_t allocated) {
  assert(get_prologue() <= header_addr && header_addr <= get_epilogue());
  assert((header_addr & 0x3) == 0x0);
  *(uint32_t *)header_addr &= 0xFFFFFFFE;
  *(uint32_t *)header_addr |= (allocated & 0x1);
}

uint32_t get_payload(uint32_t addr) {
  assert(get_firstblock() <= addr && addr < get_epilogue());
  /* addr can be:
   *    1. starting address of the block (8 * n + 4)
   *    2. starting address of the payload (8 * m)
   */
  assert((addr & 0x3) == 0);
  return _align_up(addr, 8);
}

uint32_t get_header(uint32_t addr) {
  assert(get_prologue() <= addr && addr <= get_epilogue());
  /* addr can be:
   *    1. starting address of the block (8 * n + 4)
   *    2. starting address of the next payload (8 * (n + 1))
   */
  assert((addr & 0x3) == 0);
  return _align_up(addr, 8) - 4;
}

uint32_t get_footer(uint32_t addr) {
  assert(get_prologue() <= addr && addr <= get_epilogue());
  /* addr can be:
   *    1. starting address of the block (8 * n + 4)
   *    2. starting address of the payload (8 * m)
   */
  assert(addr % 8 == 4);
  uint32_t header_addr = get_header(addr);
  uint32_t footer_addr = header_addr + get_blocksize(header_addr) - 4;
  assert(get_firstblock() < footer_addr);
  assert(footer_addr < get_epilogue());
  assert(footer_addr % 8 == 0);
  return footer_addr;
}

uint32_t get_nextheader(uint32_t addr) {
  /*
   * addr can be:
   *    1. starting address of block, the header start
   *    2. starting address of the payload
   */
  uint32_t header_addr = get_header(addr);
  uint32_t block_size = get_blocksize(header_addr);
  uint32_t next_header_addr = header_addr + block_size;
  assert(alloc_start <= next_header_addr && next_header_addr <= alloc_end - 12);
  return next_header_addr;
}

uint32_t get_prevheader(uint32_t addr) {
  /*
   * addr can be:
   *    1. starting address of block, the header start
   *    2. starting address of the payload
   */
  assert(get_firstblock() <= addr && addr <= get_epilogue());
  uint32_t header_addr = get_header(addr);
  // assert its not the first block
  assert(header_addr - alloc_start >= 12);
  uint32_t prev_footer_addr = header_addr - 4;
  uint32_t prev_blocksize = get_blocksize(prev_footer_addr);
  uint32_t prev_header_addr = header_addr - prev_blocksize;
  return prev_header_addr;
}

uint32_t get_firstblock() { return get_prologue() + 8; }
uint32_t get_lastblock() { return get_prevheader(get_epilogue()); }

