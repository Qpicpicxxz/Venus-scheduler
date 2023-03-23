#include "allocator.h"
#include "types.h"

/*
 * Boundary: The boundary of the block, header or footer,
 * 	we design the block's size is always 8-byte aligned,
 * 	so the low 3 digit can be used as some flag bits.
 *    +---------------------------+-------+
 *    |        block_size         |  000  |
 *    +---------------------------+-------+
 *	1. lowerest bit represent allocated or not
 *	2. reserve
 *	3. reserve
 *
 * Note: Make sure that the start address of the payload is 8-byte aligned
 * 	and the boundary (32bit) address is 4-byte aligned
 *
 * Block: Consists of a footer, payload and a header
 *      1. header and footer size are 4 byte (uint32_t)
 *      2. payload size should be 8-byte aligned
 *    +-----------------------------------+
 *    |               footer              | 32bits 4byte
 *    +-----------------------------------+ %8 == 0 aligned
 *    |                                   |
 *    |                                   |
 *    |               payload             |
 *    |                                   |
 *    |                                   |
 *    +-----------------------------------+ %8 == 0 aligned
 *    |               header              |
 *    +-----------------------------------+ %8 == 4 aligned
 */

extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/* Round to multiple of n */
uint32_t _align_up(uint32_t x, uint32_t n) { return n * ((x + n - 1) / n); }
uint32_t _align_down(uint32_t x, uint32_t n) { return n * (x / n); }

inline uint32_t get_prologue() { return alloc_start + 4; }
inline uint32_t get_epilogue() { return alloc_end - 4; }

/* Block operations */
#define AF_BIT (0)
#define P8_BIT (1)
#define B8_BIT (2)

/* boundary_addr: applicapable for both header_addr and footer_addr */
uint32_t get_blocksize(uint32_t boundary_addr) {
  /* if boundary_addr is an illegeal address */
  if (boundary_addr == 0) {
    return 0;
  }
  /* make sure the block_size is a multiple of 8-byte */
  return (*(uint32_t*)boundary_addr & 0xFFFFFFF8);
}

/* blocksize is the total size of header, footer and payload */
void set_blocksize(uint32_t boundary_addr, uint32_t blocksize) {
  /* if boundary_addr is an illegeal address */
  if (boundary_addr == 0) {
    return;
  }
  assert((boundary_addr & 0x03) == 0x0); // 4 byte aligned
  assert((blocksize & 0x7) == 0x0);      // 8 byte aligned
  /* mask the low 3 digit */
  *(uint32_t*)boundary_addr &= 0x7;
  /* recombine new boundary (blocksize + flags) */
  *(uint32_t*)boundary_addr |= blocksize;
}

uint32_t get_allocated(uint32_t boundary_addr) {
  if (boundary_addr == 0) {
    /* NULL could be considered as allocated */
    return ALLOCATED;
  }
  /* make sure this address is a boundary address (4-byte aligned) */
  assert((boundary_addr & 0x3) == 0x0);
  return (*(uint32_t*)boundary_addr & 0x1);
}

void set_allocated(uint32_t boundary_addr, uint32_t allocated) {
  /* if boundary_addr is an illegeal address */
  if (boundary_addr == 0) {
    return;
  }
  /* clear the last bit of boundary_addr */
  *(uint32_t*)boundary_addr &= 0xFFFFFFFE;
  /* set the last bit of boundary_addr into allocated (1 or 0) */
  *(uint32_t*)boundary_addr |= (allocated & 0x1);
}

uint32_t get_payload(uint32_t hp_addr) {
  /* make sure this address is 4-byte aligned (boundary or payload addr) */
  assert((hp_addr & 0x3) == 0);
  return _align_up(hp_addr, 8);
}

uint32_t get_header(uint32_t hp_addr) {
  assert((hp_addr & 0x3) == 0);
  return _align_up(hp_addr, 8) - 4;
}

uint32_t get_footer(uint32_t hp_addr) {
  uint32_t header_addr = get_header(hp_addr);
  uint32_t footer_addr = header_addr + get_blocksize(header_addr) - 4;
  // assert(get_firstblock() < footer_addr);
  assert(footer_addr < get_epilogue());
  assert(footer_addr % 8 == 0);
  return footer_addr;
}

uint32_t get_nextheader(uint32_t hp_addr) {
  /* make sure its not epilogue */
  assert(hp_addr < get_epilogue());
  uint32_t header_addr = get_header(hp_addr);
  uint32_t block_size = get_blocksize(header_addr);
  uint32_t next_header_addr = header_addr + block_size;
  return next_header_addr;
}

uint32_t get_prevheader(uint32_t hp_addr) {
  /* make sure its not prologue */
  assert(hp_addr > get_prologue());
  uint32_t header_addr = get_header(hp_addr);
  /* assert its not the first block */
  assert(header_addr - alloc_start >= 12);
  uint32_t prev_footer_addr = header_addr - 4;
  uint32_t prev_blocksize = get_blocksize(prev_footer_addr);
  uint32_t prev_header_addr = header_addr - prev_blocksize;
  return prev_header_addr;
}

inline uint32_t get_firstblock() { return get_prologue() + 8; }
inline uint32_t get_lastblock() { return get_prevheader(get_epilogue()); }

