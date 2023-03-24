#include "allocator.h"

uint32_t get_prevfree(uint32_t block_header) {
  if(!block_header){ return 0; }
  assert(get_firstblock() <= block_header && block_header <= get_lastblock());
  assert(get_blocksize(block_header) >= MIN_BLOCKSIZE);
  return *(uint32_t*)(block_header + 4);
}

uint32_t get_nextfree(uint32_t block_header) {
  if(!block_header){ return 0; }
  printf(YELLOW("get nextfree blocksize: %d\n"), get_blocksize(block_header));
  assert(get_blocksize(block_header) >= MIN_BLOCKSIZE);
  return *(uint32_t*)(block_header + 8);
}

void set_prevfree(uint32_t block_header, uint32_t prev_header) {
  *(uint32_t*)(block_header + 4) = (uint32_t)prev_header;
}

void set_nextfree(uint32_t block_header, uint32_t prev_header) {
  *(uint32_t*)(block_header + 8) = (uint32_t)prev_header;
}

void free_list_insert(uint32_t* list_head, uint32_t* counter_ptr, uint32_t block_header) {
  printf(YELLOW("befor insert cnt %d\n"), *counter_ptr);
  // make sure its a block header address
  assert(block_header % 8 == 4);
  assert(get_firstblock() <= block_header && block_header <= get_lastblock());
  assert(get_blocksize(block_header) >= MIN_BLOCKSIZE);
  // if list is empty
  if ((*list_head) == 0 || (*counter_ptr) == 0) {
    assert((*list_head) == 0);
    assert((*counter_ptr) == 0);
    set_prevfree(block_header, block_header);
    set_nextfree(block_header, block_header);
    (*list_head) = block_header;
    (*counter_ptr) = 1;
    return;
  }
  // is list is not empty
  uint32_t head = (*list_head);
  uint32_t tail = get_prevfree(head);
  printf("list head 0x%x\n", *list_head);
  printf("block header 0x%x\n", block_header);
  printf("tail 0x%x\n", tail);

  set_nextfree(block_header, head);
  set_prevfree(head, block_header);
  set_nextfree(tail, block_header);
  set_prevfree(block_header, tail);

  (*list_head) = block_header;
  (*counter_ptr) += 1;
}

void free_list_delete(uint32_t* list_head, uint32_t* counter_ptr, uint32_t block_header) {
  printf(YELLOW("befor delete cnt %d\n"), *counter_ptr);
  // make sure its a block header address
  assert(block_header % 8 == 4);
  assert(get_firstblock() <= block_header && block_header <= get_lastblock());
  assert(get_blocksize(block_header) >= MIN_BLOCKSIZE);
  if ((*list_head) == 0 || (*counter_ptr) == 0) {
    assert(free_list_head == 0);
    assert((*counter_ptr) == 0);
    return;
  }
  if ((*counter_ptr) == 1) {
    printf("get_prevfree((*list_head)) = 0x%x\n", get_prevfree((*list_head)));
    printf("(*list_head) = 0x%x\n", (*list_head));
    assert(get_nextfree((*list_head)) == (*list_head));
    assert(get_prevfree((*list_head)) == (*list_head));
    (*list_head) = 0;
    (*counter_ptr) = 0;
    return;
  }
  // counter >= 2
  uint32_t prev_header = get_prevfree(block_header);
  uint32_t next_header = get_nextfree(block_header);
  // if want to delete list header, assign header next block as the new header
  if (block_header == (*list_head)){
    (*list_head) = next_header;
  }
  // delete current free block from list
  set_prevfree(next_header, prev_header);
  set_nextfree(prev_header, next_header);
  (*counter_ptr) -= 1;
}

uint32_t merge_free_blocks(uint32_t low_header, uint32_t high_header) {
  // make sure they are legal merge blocks
  printf(PINK("MAEGING..........\n"));
  assert(low_header%8==4);
  assert(high_header %8 ==4);
  assert(get_firstblock() <= low_header && low_header < get_lastblock());
  assert(get_firstblock() < high_header && high_header <= get_lastblock());
  assert(get_nextheader(low_header) == high_header);
  assert(get_prevheader(high_header) == low_header);
  // set merged block's header
  uint32_t blocksize = get_blocksize(low_header) + get_blocksize(high_header);
  printf(PINK("blocksize %d\n"), blocksize);
  set_blocksize(low_header, blocksize);
  set_allocated(low_header, FREE);
  // set merged block's footer
  uint32_t footer = get_footer(high_header);
  set_blocksize(footer, blocksize);
  set_allocated(footer, FREE);
  return low_header;
}
