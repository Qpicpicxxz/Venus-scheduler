#include "allocator.h"
uint32_t alloc_start;
uint32_t alloc_end;

void heap_init() {
  alloc_start = _align_up(HEAP_START, 8);
  alloc_end = _align_down(HEAP_START + HEAP_SIZE, 8);
  // clear the heap area of data
  for (int i = alloc_start; i < alloc_end - 4; i += 4) {
    *(uint32_t*)i = 0;
  }
  uint32_t HEAP = alloc_end - alloc_start;
  printf("ALLOC:   0x%x -> 0x%x", alloc_start, alloc_end);
  printf("    SIZE:   0x%x\n", HEAP);
  /*
   *  Mark prologue allocated                    firstblock
   *  ----+---------------+----------+----------+----------+---
   *  ... |               |  header  |  footer  |  header  | ...
   *  ----+---------------+----------+----------+----------+---
   * alloc_start  prologue(+4)       +8        +12        +16
   */
  uint32_t prologue_header = get_prologue(); // alloc_start + 4
  set_allocated(prologue_header, ALLOCATED);
  set_blocksize(prologue_header, 8);
  uint32_t prologue_footer = prologue_header + 4;
  set_allocated(prologue_footer, ALLOCATED);
  set_blocksize(prologue_footer, 8);
  /*
   *  Mark epilogue allocated, has no footer
   *  ------+--------------------+
   *   ...  |       header       |
   *  ------+--------------------+
   *     epilogue(-4)       alloc_end
   */
  uint32_t epilogue_header = get_epilogue(); // alloc_end - 4
  set_allocated(epilogue_header, ALLOCATED);
  set_blocksize(epilogue_header, 0);
  /*
   *   ---+-----------+---------------------+--------------+
   *      |  prologue |    regularlblock    |   epilogue   |
   *   ---+-----------+---------------------+--------------+
   * alloc_start + 4                                  allock_end
   * get_firstblock -> get_prologue() + 8 -> regularblock
   * get_lastblock  -> get_prevblock(get_epilogue()) -> regularblock
   */
  uint32_t first_header = get_firstblock(); // get_prologue() + 8
  set_allocated(first_header, FREE);
  set_blocksize(first_header, HEAP - 4 - 8 - 4);
  uint32_t first_footer = get_footer(first_header);
  set_allocated(first_footer, FREE);
  set_blocksize(first_footer, HEAP - 4 - 8 - 4);
}

static uint32_t try_alloc_with_splitting(uint32_t block, uint32_t request_blocksize) {
  // get current block attribute
  uint32_t blocksize = get_blocksize(block);
  uint32_t allocated = get_allocated(block);
  if (allocated == FREE && blocksize >= request_blocksize) {
    // allocate this block
    if (blocksize > request_blocksize) {
      // split this block
      set_allocated(block, ALLOCATED);
      set_blocksize(block, request_blocksize);
      // set the left splitted block attribute
      uint32_t next_block_addr = block + request_blocksize;
      set_allocated(next_block_addr, FREE);
      set_blocksize(next_block_addr, blocksize - request_blocksize);
      return get_payload(block);
    } else {
      // do not need to split the block
      set_allocated(block, ALLOCATED);
      return get_payload(block);
    }
  }
  return 0;
}

void* malloc(uint32_t size) {
  // make sure it's a legal size
  assert(0 < size && size < alloc_end - alloc_start - 4 - 8 - 4);
  // payload size + header size + footer size
  uint32_t request_blocksize = _align_up(size, 8) + 4 + 4;
  uint32_t payload = 0;
  uint32_t block = get_firstblock();
  // traverse all the blocks in heap
  while (block < get_epilogue()) {
    // try to allocate
    payload = try_alloc_with_splitting(block, request_blocksize);
    if (payload) {
      return (void*)payload;
    } else {
      // go to the next block
      block = get_nextheader(block);
    }
  }
  return NULL;
}

void free(void* ptr) {
  uint32_t addr = (uint32_t)ptr;
  /* make sure this address is 8-byte aligned (payload addr) */
  assert((addr & 0x7) == 0x0);
  /* make sure we do not free prologue and epilogue */
  assert(addr > get_prologue() && addr < get_epilogue());
  uint32_t req = get_header(addr);
  uint32_t req_footer = get_footer(req);
  uint32_t req_allocated = get_allocated(req);
  uint32_t req_blocksize = get_blocksize(req);
  /* make sure this is an allocated block */
  assert(req_allocated == ALLOCATED);
  uint32_t next_header = get_nextheader(addr);
  uint32_t prev_header = get_prevheader(addr);
  uint32_t next_footer = get_footer(next_header);
  // uint32_t prev_footer = get_footer(prev_header);
  uint32_t next_allocated = get_allocated(next_header);
  uint32_t prev_allocated = get_allocated(prev_header);
  uint32_t next_blocksize = get_blocksize(next_header);
  uint32_t prev_blocksize = get_blocksize(prev_header);
  /*
   * Case 1: A--(A->F)--A ==> AFA
   * ----+------------+------+------------+----
   *  A  | req_header | FREE | req_footer |  A
   * ----+------------+------+------------+----
   */
  if (next_allocated == ALLOCATED && prev_allocated == ALLOCATED) {
    // free current block
    set_allocated(req, FREE);
    set_allocated(req_footer, FREE);
    /*
     * Case 2: A--(A->F)--FA  ==> AFFA
     * ----+------------+------+-------------+----
     *  A  | req_header | FREE | next_footer |  A
     * ----+------------+------+-------------+----
     */
  } else if (next_allocated == FREE && prev_allocated == ALLOCATED) {
    // marge current block and next free block
    set_allocated(req, FREE);
    set_allocated(next_footer, FREE);
    set_blocksize(req, req_blocksize + next_blocksize);
    set_blocksize(next_footer, req_blocksize + next_blocksize);
    /*
     * Case 3: AF--(A->F)--A ==> AFFA
     * ----+-------------+------+------------+----
     *  A  | prev_header | FREE | req_footer |  A
     * ----+-------------+------+------------+----
     */
  } else if (next_allocated == ALLOCATED && prev_allocated == FREE) {
    // merge current block and previous free block
    set_allocated(prev_header, FREE);
    set_allocated(req_footer, FREE);
    set_blocksize(prev_header, req_blocksize + prev_blocksize);
    set_blocksize(req_footer, req_blocksize + prev_blocksize);
    /*
     * Case 4: AF--(A->F)--FA ==> AFFFA
     * ----+-------------+------+-------------+----
     *  A  | prev_header | FREE | next_footer |  A
     * ----+-------------+------+-------------+----
     */
  } else {
    // merge current, next and previous free block
    set_allocated(prev_header, FREE);
    set_allocated(next_footer, FREE);
    set_blocksize(prev_header, req_blocksize + prev_blocksize + next_blocksize);
    set_blocksize(next_footer, req_blocksize + prev_blocksize + next_blocksize);
  }
}

void malloc_test() {
  printf(YELLOW("\nTesting Malloc...\n"));
  void* p = malloc(1);
  printf("p = 0x%x\n", p);
  void* p1 = malloc(sizeof(int));
  printf("p1 = 0x%x\n", p1);
  void* p2 = malloc(1000000);
  printf("p2 = 0x%x\n", p2);
  free(p);
  free(p1);
  free(p2);
  p1 = malloc(1);
  assert(p == p1);
  printf(GREEN("Malloc Pass\n"));
}
