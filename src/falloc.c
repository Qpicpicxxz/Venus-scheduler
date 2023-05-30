#include "common.h"

extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/*
 * _alloc_start points to the actual start address of heap pool
 * _alloc_end points to the actual end address of heap pool
 * _num_pages holds the actual max number of pages we can allocate.
 */
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end   = 0;
static uint32_t _num_pages   = 0;

#define PAGE_SIZE  256
#define PAGE_ORDER 8
#define META_SIZE  2048

#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST  (uint8_t)(1 << 1)

/*
 * Page Descriptor
 * flags:
 * - bit 0: flag if this page is taken(allocated)
 * - bit 1: flag if this page is the last page of the memory block allocated
 */
struct Page {
  uint8_t flags;
};

static inline void _clear(struct Page* page) {
  page->flags = 0;
}

static inline int _is_free(struct Page* page) {
  if (page->flags & PAGE_TAKEN) {
    return 0;
  } else {
    return 1;
  }
}

static inline void _set_flag(struct Page* page, uint8_t flags) {
  page->flags |= flags;
}

static inline int _is_last(struct Page* page) {
  if (page->flags & PAGE_LAST) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * align the address to the border of page(4K)
 */
static inline uint32_t _align_page(uint32_t address) {
  uint32_t order = (1 << PAGE_ORDER) - 1;
  return (address + order) & (~order);
}

void heap_init() {
  /*
   * We reserved 8 Page (8 x 4096) to hold the Page structures.
   * It should be enough to manage at most 128 MB (8 x 4096 x 4096)
   */
  _num_pages = (HEAP_SIZE / PAGE_SIZE) - META_SIZE;
  _num_pages = 1000;

  struct Page* page = (struct Page*)HEAP_START;
  for (int i = 0; i < _num_pages; i++) {
    _clear(page);
    page++;
  }

  _alloc_start = _align_page(HEAP_START + META_SIZE * PAGE_SIZE);
  _alloc_end   = _alloc_start + (PAGE_SIZE * _num_pages);
}

/*
 * Allocate a memory block which is composed of contiguous physical pages
 * - npages: the number of PAGE_SIZE pages to allocate
 */
void* malloc(uint32_t size) {
  /* Note we are searching the page descriptor bitmaps. */
  int npages          = (size - 1) / PAGE_SIZE + 1;
  int found           = 0;
  struct Page* page_i = (struct Page*)HEAP_START;
  for (int i = 0; i <= (_num_pages - npages); i++) {
    if (_is_free(page_i)) {
      found = 1;
      /*
       * meet a free page, continue to check if following
       * (npages - 1) pages are also unallocated.
       */
      struct Page* page_j = page_i + 1;
      for (int j = i + 1; j < (i + npages); j++) {
        if (!_is_free(page_j)) {
          found = 0;
          break;
        }
        page_j++;
      }
      /*
       * get a memory block which is good enough for us,
       * take housekeeping, then return the actual start
       * address of the first page of this memory block
       */
      if (found) {
        struct Page* page_k = page_i;
        for (int k = i; k < (i + npages); k++) {
          _set_flag(page_k, PAGE_TAKEN);
          page_k++;
        }
        page_k--;
        _set_flag(page_k, PAGE_LAST);
        return (void*)(_alloc_start + i * PAGE_SIZE);
      }
    }
    page_i++;
  }
  return NULL;
}

/*
 * Free the memory block
 * - p: start address of the memory block
 */
void free(void* ptr) {
  /*
   * Assert (TBD) if p is invalid
   */
  if (!ptr || (uint32_t)ptr >= _alloc_end) {
    return;
  }
  /* get the first page descriptor of this memory block */
  struct Page* page = (struct Page*)HEAP_START;
  page += ((uint32_t)ptr - _alloc_start) / PAGE_SIZE;
  /* loop and clear all the page descriptors of the memory block */
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

void* malloc_LLI() {
  return malloc(64);
}

void free_LLI(void* ptr) {
  free(ptr);
}

void heap_test() {
  printf("alloc_start: %p\n", _alloc_start);
  void* p1 = malloc(1);
  void* p2 = malloc(1);
  printf("p1, p2: %p, %p\n", p1, p2);
  void* p3 = malloc_LLI();
  printf("p3: %p\n", p3);
  free(p1);
  free(p2);
  free_LLI(p3);
  void* p4 = malloc(1);
  printf("p4: %p\n", p4);
}
