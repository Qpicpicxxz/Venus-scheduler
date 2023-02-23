// Achieve dynamic memory allocation and release

#include "page.h"

/*
 _alloc_start --- the actual start address of heap pool
 _alloc_end --- the actual end address of heap pool
 _num_pages --- the actual max number of pages we can allocate.
*/
static uint32_t _num_pages = 0;
static uint32_t _alloc_end = 0;

/*
         we reserved 8 Page (8 x 4096) to hold the Page structures
         it should be enough to manage at most 128 MB (8 x 4096 x 4096)
         page initialze to allocate page control infomation and store blocks
*/
void page_init() {
  // the first META_SIZE pages can be used for metadata
  _num_pages = (HEAP_SIZE / PAGE_SIZE) - META_SIZE;

  struct Page *page = (struct Page *)HEAP_START;
  for (uint32_t i = 0; i < _num_pages; i++) {
    _clear(page);
    page++;
  }
  // heap block start address despite the page table descriptor
  // _alloc_start aligned to the beginnning of the 9th page
  _alloc_start = _align_page(HEAP_START + META_SIZE * PAGE_SIZE);
  _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);
  uint32_t HEAP = _alloc_end - _alloc_start;
  printf(
      "\nHeap section:  start = 0x%x    SIZE:   0x%x\nTotal num of pages: %d\n",
      HEAP_START, HEAP_SIZE, _num_pages);
  printf("\nALLOC:   0x%x -> 0x%x", _alloc_start, _alloc_end);
  printf("    SIZE:   0x%x\n", HEAP);
}

// allocate a memory block which is composed of contiguous physical pages
// parameter --- npages: the number of PAGE_SIZE pages to allocate
void *malloc(int bit) {
  // searching the page descriptor bitmaps
  int npages = pageNum(bit);
  int found = 0;
  struct Page *page_i = (struct Page *)HEAP_START;
  for (int i = 0; i <= (_num_pages - npages); i++) {
    if (_is_free(page_i)) {
      found = 1;
      // encounter a free page,
      // continue to check if the following (npages - 1) pages are also
      // unallocated.
      struct Page *page_j = page_i + 1;
      for (int j = i + 1; j < (i + npages); j++) {
        if (!_is_free(page_j)) {
          found = 0;
          break;
        }
        page_j++;
      }
      // get a memory block which is good enough for us and address of the first
      // page of this memory block
      if (found) {
        struct Page *page_k = page_i;
        for (int k = i; k < (i + npages); k++) {
          _set_flag(page_k, PAGE_TAKEN);
          page_k++;
        }
        page_k--;
        _set_flag(page_k, PAGE_LAST);
        return (void *)(_alloc_start + i * PAGE_SIZE);
      }
    }
    page_i++;
  }
  return NULL;
}

// free the memory block, p represents the start address of the memory block
void free(void *ptr) {
  if (!ptr || (uint32_t)ptr >= _alloc_end) {
    return;
  }
  // get the first page descriptor of this memory block
  struct Page *page = (struct Page *)HEAP_START;
  page += ((uint32_t)ptr - _alloc_start) / PAGE_SIZE;
  // loop and clear all the page descriptors of the memory block
  while (!_is_free(page)) {
    if (_is_last(page)) {
      _clear(page);
      break;
    } else {
      _clear(page);
      page++;
      ;
    }
  }
}

void page_test() {
  // would print out the start address of the heap pointer
  void *p = malloc(1);
  printf("p = 0x%x\n", p);	// suppose is 0x80099500
  void *p1 = malloc(128);
  printf("p1 = 0x%x\n", p1);	// 0x80099600
  void *p2 = malloc(256);
  printf("p2 = 0x%x\n", p2);	// 0x80099700
  void *p3 = malloc(513);
  printf("p3 = 0x%x\n", p3);	// 0x80099800
  /*
     the basic allocate size is 256bit ---> 0x0000_0100
        ⌈513 / 256⌉ =  3 times larger
     the malloc will allocates 3 blocks for the request
     base_addr + 0x0000_0300 ---> 8 + 3 = b
     Before Allocation:

	+----+    +----+    +----+    +----+    +----+
	| 00 | -> | 00 | -> | 00 | -> | 00 | -> | 00 |
	+----+    +----+    +----+    +----+    +----+

     After Allocation:

	+----+    +----+    +----+    +----+    +----+
	| 01 | -> | 01 | -> | 11 | -> | 00 | -> | 00 |
	+----+    +----+    +----+    +----+    +----+
  */
  void *p4 = malloc(2048);
  printf("p4 = 0x%x\n", p4);	// 0x80099b00
  free(p);
  free(p1);
  free(p2);
  free(p3);
  free(p4);
}

