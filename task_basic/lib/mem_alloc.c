/* Ref: https://github.com/yangminz/bcst_csapp/blob/main/src/malloc/mem_alloc.c */
#include "allocator.h"

uint32_t alloc_start;
uint32_t alloc_end;
uint32_t free_list_head    = 0;
uint32_t free_list_counter = 0;

void init_sentinel(void) {
  /*
   *  Mark prologue allocated                    firstblock
   *  ----+---------------+----------+----------+----------+---
   *  ... |     cover     |  header  |  footer  |  header  | ...
   *  ----+---------------+----------+----------+----------+---
   * alloc_start  prologue(+4)       +8        +12        +16
   */
  WRITE_BURST_32(get_prologue(), 0, 0);
  uint32_t prologue_header = get_prologue();  // alloc_start + 4
  set_allocated(prologue_header, ALLOCATED);
  set_blocksize(prologue_header, 8);
  WRITE_BURST_32(prologue_header, 4, 0);
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
  WRITE_BURST_32(get_epilogue(), 0, 0);
  uint32_t epilogue_header = get_epilogue();  // alloc_end - 4
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
  WRITE_BURST_32(get_firstblock(), 0, 0);
  uint32_t first_header = get_firstblock();  // get_prologue() + 8
  set_allocated(first_header, FREE);
  set_blocksize(first_header, alloc_end - alloc_start - 4 - 8 - 4);
  WRITE_BURST_32(get_footer(first_header), 0, 0);
  uint32_t first_footer = get_footer(first_header);
  set_allocated(first_footer, FREE);
  set_blocksize(first_footer, alloc_end - alloc_start - 4 - 8 - 4);
}

void heap_init(void) {
  alloc_start = _align_up(HEAP_START, 8);
  alloc_end   = _align_down(HEAP_START + HEAP_SIZE, 8);
  /* Clear heap area to zero */
  // for (int i = alloc_start; i < alloc_end - 4; i += 4) {
  //   *(uint32_t*)i = 0;
  // }
  // memset(alloc_start, 0, alloc_end - alloc_start);
  /* prologue + alloc area + epilogue */
  init_sentinel();
  uint32_t first_block = get_firstblock();
  /* Bidirectional ring linked list */
  set_prevfree(first_block, first_block);
  set_nextfree(first_block, first_block);
  free_list_head    = 0;
  free_list_counter = 0;
  /* Insert initial free block into free list */
  free_list_insert(first_block);
}

static uint32_t try_alloc_with_splitting(uint32_t block_header, uint32_t request_blocksize) {
  if (request_blocksize < MIN_BLOCKSIZE) {
    return 0;
  }

  uint32_t blocksize = get_blocksize(block_header);
  uint32_t allocated = get_allocated(block_header);

  if (allocated == FREE && blocksize >= request_blocksize) {
    // allocate this block
    if (blocksize - request_blocksize >= MIN_BLOCKSIZE) {
      /*
       * Before split:
       *       |<------------------------------------------ blocksize ----------------------------------------------->|
       *     --+-----------+----------+----------+--------------------------------------------------------+-----------+--
       *       | header(F) | prevfree | nextfree |                          FREE                          | footer(F) |
       *     --+-----------+----------+----------+--------------------------------------------------------+-----------+--
       *  block_header                                                                               block_footer
       *
       * After split:
       *       |<---------- request_blocksize ------------>|<------------blocksize - request_blocksize -------------->|
       *     --+-----------+-------------------+-----------+-----------+----------+----------+------------+-----------+--
       *       | header(A) |      payload      | footer(A) | header(F) | prevfree | nextfree |    FREE    | footer(F) |
       *     --+-----------+-------------------+-----------+-----------+----------+----------+------------+-----------+--
       *  block_header                     block_footer  new_header                                    new_footer
       */
      uint32_t new_footer = get_footer(block_header);
      set_blocksize(new_footer, blocksize - request_blocksize);

      set_allocated(block_header, ALLOCATED);
      set_blocksize(block_header, request_blocksize);

      WRITE_BURST_32(get_footer(block_header), 0, 0);
      uint32_t block_footer = get_footer(block_header);
      set_allocated(block_footer, ALLOCATED);
      set_blocksize(block_footer, request_blocksize);

      WRITE_BURST_32(get_nextheader(block_header), 0, 0);
      uint32_t new_header = get_nextheader(block_header);
      set_allocated(new_header, FREE);
      set_blocksize(new_header, blocksize - request_blocksize);

      return get_payload(block_header);

    } else if (blocksize - request_blocksize < MIN_BLOCKSIZE) {
      // if splitted, it could't contain header + prevfree + nextfree + footer descriptor

      // set_allocated(block_header, ALLOCATED);
      // uint32_t block_footer = get_footer(block_header);
      // set_allocated(block_footer, ALLOCATED);

      // return get_payload(block_header);
      return 0;
    }
  }
  return 0;
}

void* malloc(uint32_t size) {
  // make sure it's a legal size
  assert(0 < size && size < alloc_end - alloc_start - 4 - 8 - 4);
  // payload size + header size + footer size
  uint32_t request_blocksize = _align_up(size, 8) + BOUNDART_SIZE;
  // make it to minimum allocatable space
  request_blocksize = request_blocksize < MIN_BLOCKSIZE ? MIN_BLOCKSIZE : request_blocksize;
  // search free block from free list head
  uint32_t block_header = free_list_head;
  // traverse all the blocks in heap
  uint32_t counter = free_list_counter;
  // initialize current block payload
  uint32_t block_payload = 0;
  for (int i = 0; i < counter; ++i) {
    // record original block size
    uint32_t old_blocksize = get_blocksize(block_header);
    // try to allocate
    block_payload = try_alloc_with_splitting(block_header, request_blocksize);

    if (block_payload) {
      // get current block size
      uint32_t cur_blocksize = get_blocksize(block_header);
      // assert(cur_blocksize <= old_blocksize);
      free_list_delete(block_header);
      // if current block has been splitted
      if (old_blocksize > cur_blocksize)
        free_list_insert(get_nextheader(block_header));
      return (void*)block_payload;
    } else {
      // go to the next free block
      block_header = get_nextfree(block_header);
    }
  }
  printf("[SCHEDULER ERROR] There is not enough HEAP memory to allocate! $stop\n");
  return NULL;
}

void free(void* ptr) {
  if (ptr == NULL) {
    printf("[SCHEDULER ERROR] Free a NULL pointer... $stop\n");
    return;
  }
  uint32_t payload_addr = (uint32_t)ptr;

  uint32_t req_header    = get_header(payload_addr);
  uint32_t req_footer    = get_footer(req_header);
  uint32_t req_allocated = get_allocated(req_header);
  uint32_t req_blocksize = get_blocksize(req_header);
  assert(req_allocated == ALLOCATED);
  assert(req_blocksize >= MIN_BLOCKSIZE);

  uint32_t next_header    = get_nextheader(req_header);
  uint32_t prev_header    = get_prevheader(req_header);
  uint32_t next_allocated = get_allocated(next_header);
  uint32_t prev_allocated = get_allocated(prev_header);

  if (next_allocated == ALLOCATED && prev_allocated == ALLOCATED) {
    /*
     * Case 1: *A(A->F)A* ---> *AFA*
     * ----+------------+------+------------+----
     *  A  | req_header | FREE | req_footer |  A
     * ----+------------+------+------------+----
     *               needn't merge
     */

    // free current block
    set_allocated(req_header, FREE);
    set_allocated(req_footer, FREE);
    free_list_insert(req_header);
  } else if (next_allocated == FREE && prev_allocated == ALLOCATED) {
    /*
     * Case 2: *A(A->F)FA* ---> AFFA ---> *A[FF]A*
     * ----+------------+------+-------------+----
     *  A  | req_header | FREE | next_footer |  A
     * ----+------------+------+-------------+----
     *           merge current and next
     */

    free_list_delete(next_header);
    uint32_t merged_header = merge_free_blocks(req_header, next_header);
    free_list_insert(merged_header);

  } else if (next_allocated == ALLOCATED && prev_allocated == FREE) {
    /*
     * Case 3: *AF(A->F)A* ---> AFFA ---> *A[FF]A*
     * ----+-------------+------+------------+----
     *  A  | prev_header | FREE | req_footer |  A
     * ----+-------------+------+------------+----
     *         merge previous and current
     */

    free_list_delete(prev_header);
    uint32_t merged_header = merge_free_blocks(prev_header, req_header);
    free_list_insert(merged_header);

  } else if (next_allocated == FREE && prev_allocated == FREE) {
    /*
     * Case 4: *AF(A->F)FA* ---> AFFFA ---> *A[FFF]A*
     * ----+-------------+------+-------------+----
     *  A  | prev_header | FREE | next_footer |  A
     * ----+-------------+------+-------------+----
     *      merge current, previous and next
     */

    free_list_delete(next_header);
    free_list_delete(prev_header);
    uint32_t merged_header = merge_free_blocks(merge_free_blocks(prev_header, req_header), next_header);
    free_list_insert(merged_header);
    assert(free_list_counter >= 1);
  } else {
    printf("[SCHEDULER ERROR] Exception for free ptr $stop\n");
  }
}

