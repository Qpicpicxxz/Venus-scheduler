/* ref: https://github.com/yangminz/bcst_csapp/blob/bcb8ddbcf542b9f196868aa35661223a2f40eb25/src/mains/mem_alloc.c */
#include "allocator.h"

uint32_t get_prevfree(uint32_t block_header) {
  if (!block_header) {
    return 0;
  }
  return *(uint32_t*)(block_header + 4);
}

uint32_t get_nextfree(uint32_t block_header) {
  if (!block_header) {
    return 0;
  }
  return *(uint32_t*)(block_header + 8);
}

void set_prevfree(uint32_t block_header, uint32_t prev_header) {
  *(uint32_t*)(block_header + 4) = (uint32_t)prev_header;
}

void set_nextfree(uint32_t block_header, uint32_t prev_header) {
  *(uint32_t*)(block_header + 8) = (uint32_t)prev_header;
}

void free_list_insert(uint32_t block_header) {
  // if list is empty
  if (free_list_head == 0 || free_list_counter == 0) {
    set_prevfree(block_header, block_header);
    set_nextfree(block_header, block_header);
    free_list_head    = block_header;
    free_list_counter = 1;
    return;
  }
  /*
   * Next free list:
   *                +--------------------------------------+
   *     Before:    |                                      |
   *                +-->old_head--->node--->node--->tail---+
   *
   *
   *                +------------------------------------------------------+
   *     After:     |                                                      |
   *                +-->block_header--->old_head--->node--->node--->tail---+
   *                     (new_head)
   */
  uint32_t tail = get_prevfree(free_list_head);

  set_nextfree(block_header, free_list_head);
  set_prevfree(free_list_head, block_header);
  set_nextfree(tail, block_header);
  set_prevfree(block_header, tail);

  free_list_head = block_header;
  free_list_counter += 1;
}

void free_list_delete(uint32_t block_header) {
  if (free_list_head == 0 || free_list_counter == 0) {
    return;
  }
  if (free_list_counter == 1) {
    free_list_head    = 0;
    free_list_counter = 0;
    return;
  }

  uint32_t prev_header = get_prevfree(block_header);
  uint32_t next_header = get_nextfree(block_header);

  // if want to delete list header, assign header next block as the new header
  if (block_header == free_list_head) {
    free_list_head = next_header;
  }
  /*
   * prev_header <---> block_header <---> next_header
   * prev_header <---> next_header
   */
  set_prevfree(next_header, prev_header);
  set_nextfree(prev_header, next_header);
  free_list_counter -= 1;
}

uint32_t merge_free_blocks(uint32_t low_header, uint32_t high_header) {
  /*
   *
   * Before merge:
   *       |<------------- low_blocksize ------------->|<------------- high_blocksize ------------>|
   *     --+-----------+-------------------+-----------+-----------+-------------------+-----------+
   *       | header(?) |                   | footer(?) | header(?) |                   | footer(?) |
   *     --+-----------+-------------------+-----------+-----------+-------------------+-----------+
   *    low_header                                                                             high_footer
   *
   * After merge:
   *       |<--------------------------- low_blocksize + high_blocksize -------------------------->|
   *     --+-----------+---------------------------------------------------------------+-----------+
   *       | header(F) |                                                               | footer(F) |
   *     --+-----------+---------------------------------------------------------------+-----------+
   */
  uint32_t blocksize = get_blocksize(low_header) + get_blocksize(high_header);
  set_blocksize(low_header, blocksize);
  set_allocated(low_header, FREE);

  // set merged block's footer
  uint32_t high_footer = get_footer(high_header);
  set_blocksize(high_footer, blocksize);
  set_allocated(high_footer, FREE);

  return low_header;
}

