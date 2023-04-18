/* ref: https://github.com/yangminz/bcst_csapp/blob/main/src/malloc/block.c */
#include "allocator.h"

/*
 * Boundary: The boundary of the block, header or footer,
 * 	we design the block's size is always 8-byte(DWORD) aligned,
 * 	so the low 3 digit can be used as some flag bits.
 *    1. lowerest bit represent allocated or not
 *    2. reserve
 *    3. reserve
 *    +---------------------------+-------+
 *    |        block_size         |  000  | 32bits == 4byte
 *    +---------------------------+-------+
 *
 * Note: Make sure that the start address of the payload is 8-byte aligned
 * 	and the boundary (32bit) address is 4-byte aligned
 *
 * Free block consists of:
 *    1. a footer, 4 bytes
 *    2. a header, 4 bytes
 *    3. a successor, the next free block header, 4 bytes
 *    4. a predecessor, the previous freeblock header, 4 bytes
 *    +-----------------------------------+
 *    |               footer              |
 *    +-----------------------------------+ %8 == 0 aligned
 *    |                                   |
 *    |                                   |
 *    |                FREE               |
 *    |                                   |
 *    |                                   |
 *    +-----------------------------------+ %8 == 0 aligned
 *    |              successor            |
 *    +-----------------------------------+ %8 == 4 aligned
 *    |             predecessor           |
 *    +-----------------------------------+ %8 == 0 aligned
 *    |               header              |
 *    +-----------------------------------+ %8 == 4 aligned
 *    Therefore, MIN_BLOCKSIZE = 16
 *
 * Allocated block consists of:
 *    1. a header, 4 bytes
 *    2. a footer, 4 bytes
 *    3. a payload
 *    4. a padding, to make sure the size of the block is 8-byte aligned
 *    +-----------------------------------+
 *    |               footer              |
 *    +-----------------------------------+ %8 == 0 aligned
 *    |                                   |
 *    |               padding             |
 *    |                                   |
 *    +-----------------------------------+
 *    |                                   |
 *    |                                   |
 *    |                                   |
 *    |               payload             |
 *    |                                   |
 *    |                                   |
 *    |                                   |
 *    +-----------------------------------+ %8 == 0 aligned
 *    |               header              |
 *    +-----------------------------------+ %8 == 4 aligned
 */

/* Round to multiple of n */
inline uint32_t _align_up(uint32_t x, uint32_t n) { return n * ((x + n - 1) / n); }
inline uint32_t _align_down(uint32_t x, uint32_t n) { return n * (x / n); }

inline uint32_t get_prologue() { return alloc_start + 4; }
inline uint32_t get_epilogue() { return alloc_end - 4; }

/* make sure the block_size is a multiple of 8-byte */
inline uint32_t get_blocksize(uint32_t boundary_addr) { return *(uint32_t*)boundary_addr & 0xFFFFFFF8; }

/* blocksize is the total size of header, footer and payload */
inline void set_blocksize(uint32_t boundary_addr, uint32_t blocksize) { *(uint32_t*)boundary_addr = (*(uint32_t*)boundary_addr & 0x7) | blocksize; }

inline uint32_t get_allocated(uint32_t boundary_addr) { return *(uint32_t*)boundary_addr & 0x1; }

inline void set_allocated(uint32_t boundary_addr, uint32_t allocated) { *(uint32_t*)boundary_addr = (*(uint32_t*)boundary_addr & 0xFFFFFFFE) | (allocated & 0x1); }

inline uint32_t get_payload(uint32_t hp_addr) { return _align_up(hp_addr, 8); }

inline uint32_t get_header(uint32_t hp_addr) { return _align_up(hp_addr, 8) - 4; }

uint32_t get_footer(uint32_t hp_addr) {
  uint32_t header_addr = get_header(hp_addr);
  uint32_t footer_addr = header_addr + get_blocksize(header_addr) - 4;
  return footer_addr;
}

uint32_t get_nextheader(uint32_t hp_addr) {
  uint32_t header_addr      = get_header(hp_addr);
  uint32_t block_size       = get_blocksize(header_addr);
  uint32_t next_header_addr = header_addr + block_size;
  return next_header_addr;
}

uint32_t get_prevheader(uint32_t hp_addr) {
  uint32_t header_addr      = get_header(hp_addr);
  uint32_t prev_footer_addr = header_addr - 4;
  uint32_t prev_blocksize   = get_blocksize(prev_footer_addr);
  uint32_t prev_header_addr = header_addr - prev_blocksize;
  return prev_header_addr;
}

inline uint32_t get_firstblock() { return get_prologue() + 8; }
inline uint32_t get_lastblock() { return get_prevheader(get_epilogue()); }

