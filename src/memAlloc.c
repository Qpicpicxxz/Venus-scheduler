#include "allocator.h"
uint32_t alloc_start;
uint32_t alloc_end;

void heap_init() {
  alloc_start = _align_up(HEAP_START, 8);
  alloc_end = _align_down(HEAP_START + HEAP_SIZE, 8);
  for (int i = alloc_start; i < alloc_end - 4; i += 4) {
    *(uint32_t *)i = 0;
  }
  uint32_t HEAP = alloc_end - alloc_start;
  printf("ALLOC:   0x%x -> 0x%x", alloc_start, alloc_end);
  printf("    SIZE:   0x%x\n", HEAP);
  // to ensure: A--F--A
  // set the prologue block
  uint32_t prologue_header = get_prologue();
  set_allocated(prologue_header, ALLOCATED);
  set_blocksize(prologue_header, 16);
  uint32_t prologue_footer = prologue_header + 12;
  set_allocated(prologue_footer, ALLOCATED);
  set_blocksize(prologue_footer, 16);
  // set the epilogue block
  uint32_t epilogue = get_epilogue();
  set_allocated(epilogue, ALLOCATED);
  set_blocksize(epilogue, 0);

  uint32_t first_header = get_firstblock();
  set_allocated(first_header, FREE);
  set_blocksize(first_header, HEAP - 16 - 8);
  uint32_t first_footer = get_footer(first_header);
  set_allocated(first_footer, FREE);
  set_blocksize(first_footer, HEAP - 16 - 8);
}

void *malloc(uint32_t size) {
  assert(0 < size && size < HEAP_SIZE - 4 - 8 - 4);
  // align + header size + footer size
  uint32_t request_blocksize = _align_up(size, 8) + 4 + 4;
  uint32_t b = get_firstblock();
  while (b < get_lastblock()) {
    // try alloc
    uint32_t b_blocksize = get_blocksize(b);
    uint32_t b_allocated = get_allocated(b);
    if (b_allocated == 0 && b_blocksize >= request_blocksize) {
      // allocate this block
      if (b_blocksize > request_blocksize) {
        // split this block
        set_allocated(b, 1);
        set_blocksize(b, request_blocksize);

        // set the left splitted block
        uint32_t next_header_addr = b + request_blocksize;
        set_allocated(next_header_addr, 0);
        set_blocksize(next_header_addr, b_blocksize - request_blocksize);
        
        return (void *)get_payload(b);
      } else {
        // do not need to split the block
        set_allocated(b, 1);
        
        return (void *)get_payload(b);
      }
    } else {
      // go to the next block
      b = get_nextheader(b);
    }
  }
  return NULL;
}

void free(void *ptr) {
  uint32_t addr = (uint32_t)ptr;
  assert(get_firstblock() <= addr && addr <= get_epilogue());
  assert((addr & 0x7) == 0x0);
  uint32_t req = get_header(addr);
  uint32_t req_footer = get_footer(req);
  uint32_t req_allocated = get_allocated(req);
  uint32_t req_blocksize = get_blocksize(req);
  assert(req_allocated == 1);

  uint32_t next = get_nextheader(addr);
  uint32_t prev = get_prevheader(addr);
  uint32_t next_footer = get_footer(next);
  uint32_t next_allocated = get_allocated(next);
  uint32_t prev_allocated = get_allocated(prev);
  uint32_t next_blocksize = get_blocksize(next);
  uint32_t prev_blocksize = get_blocksize(prev);
  if (next_allocated == 1 && prev_allocated == 1) {
    // case 1: A--(A->F)--A ==> AFA
    set_allocated(req, 0);
    set_allocated(req_footer, 0);
  } else if (next_allocated == 0 && prev_allocated == 1) {
    // case 2: A--(A->F)--FA  ==> AFFA (merge current and next)
    set_allocated(req, 0);
    set_blocksize(req, req_blocksize + next_blocksize);
    set_allocated(next_footer, 0);
    set_blocksize(next_footer, req_blocksize + next_blocksize);
  } else if (next_allocated == 1 && prev_allocated == 0) {
    // case 3: AF--(A->F)--A ==> AFFA (merge current and prev)
    set_allocated(prev, 0);
    set_blocksize(prev, req_blocksize + prev_blocksize);
    set_allocated(req_footer, 0);
    set_blocksize(req_footer, req_blocksize + prev_blocksize);
  } else {
    // case 4: AF--(A->F)--FA ==> AFFFA (merge current, prev and next)
    set_allocated(prev, 0);
    set_blocksize(prev, req_blocksize + prev_blocksize + next_blocksize);
    set_allocated(next_footer, 0);
    set_blocksize(next_footer, req_blocksize + prev_blocksize + next_blocksize);
  }
}

void malloc_test() {
  printf("Testing round up...\n");

  for (int i = 0; i < 100; ++i) {
    for (int j = 1; j <= 8; ++j) {
      uint32_t x = i * 8 + j;
      assert(_align_up(x, 8) == (i + 1) * 8);
    }
  }

  printf(GREEN("Pass\n"));

  printf("Testing getting block size from header ...\n");

  heap_init();

  for (int i = get_prologue() + 16; i <= get_epilogue() / 512; i += 4) {
    *(uint32_t *)i = 0x1234abc0;
    assert(get_blocksize(i) == 0x1234abc0);
    assert(get_allocated(i) == FREE);

    *(uint32_t *)i = 0x1234abc1;
    assert(get_blocksize(i) == 0x1234abc0);
    assert(get_allocated(i) == ALLOCATED);

    *(uint32_t *)i = 0x1234abc8;
    assert(get_blocksize(i) == 0x1234abc8);
    assert(get_allocated(i) == FREE);

    *(uint32_t *)i = 0x1234abc9;
    assert(get_blocksize(i) == 0x1234abc8);
    assert(get_allocated(i) == ALLOCATED);
  }

  printf(GREEN("Pass\n"));

  printf("Testing setting block size to header ...\n");

  heap_init();

  for (int i = get_firstblock() + 16; i < get_epilogue() / 512; i += 4) {
    set_blocksize(i, 0x1234abc0);
    set_allocated(i, FREE);
    assert(get_blocksize(i) == 0x1234abc0);
    assert(get_allocated(i) == FREE);

    set_blocksize(i, 0x1234abc0);
    set_allocated(i, ALLOCATED);
    assert(get_blocksize(i) == 0x1234abc0);
    assert(get_allocated(i) == ALLOCATED);

    set_blocksize(i, 0x1234abc8);
    set_allocated(i, FREE);
    assert(get_blocksize(i) == 0x1234abc8);
    assert(get_allocated(i) == FREE);

    set_blocksize(i, 0x1234abc8);
    set_allocated(i, ALLOCATED);
    assert(get_blocksize(i) == 0x1234abc8);
    assert(get_allocated(i) == ALLOCATED);
  }

  // set the block size of last block
  for (int i = 2; i < 100; ++i) {
    uint32_t blocksize = i * 8;
    uint32_t addr =
        get_epilogue() - blocksize; // + 4 for the virtual footer in next page
    set_blocksize(addr, blocksize);
    assert(get_blocksize(addr) == blocksize);
  }

  printf(GREEN("Pass\n"));
}
