// Achieve dynamic memory allocation and release

#include "os.h"

/*
  _alloc_start --- the actual start address of heap pool
  _alloc_end --- the actual end address of heap pool
  _num_pages --- the actual max number of pages we can allocate.
*/
static uint32_t _num_pages = 0;
uint32_t _alloc_start = 0;
uint32_t _alloc_end = 0;


extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;


/* 
   The minimum memory allocate unit, called Chunk
   so if we call malloc(), it would at least allocate 256bits
   If want to adjust, you should modify three macro below
   the minimum metadata index size is RAM_SIZE / PAGE_SIZE^2
   Matedata is the Chunk header
   Note: The finer grained memory control would cause 
         larger matadata occupation, we need to make a tradeoff
         based on the actual situation.
*/
#define PAGE_SIZE 256  // 256 Byte
#define PAGE_ORDER 8   // 256 = 2^8
#define META_SIZE 2048 // 0x0800_0000 / (256 * 256)

/* 
  a preprocessor directive which defines a constant macro,
  (1 << 2) is bit shift operation,
  shifts the binary representation of the integer 1
  two places to the left, which represent the value of 0b00000100
  pageNum implement round up to an integer
*/
#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST (uint8_t)(1 << 1)
#define pageNum(x) ((x - 1) / (PAGE_SIZE)) + 1

/*
  Page descriptor flags:
  1. flags = 0 --- this page is allocated
  2. flags = 1 --- this page is the last page of the memory block allocated
*/
struct Page {
  uint8_t flags;
};

// get page index pointer
static inline struct Page *_get_page(void *p) {
  struct Page *page = (struct Page *)HEAP_START;
  page += ((uint32_t)p - _alloc_start) / PAGE_SIZE;
  return page;
}

// get alloc page pointer
static inline void *_get_addr(struct Page *page) {
  /*
    1. page - (sturct *Page)HEAP_START 
       calculates the offset of the given page from the beginning of the heap
    2. each page is 32 `page` structures long
    3. (num * PAGE_SIZE) to get the starting address of the page within the heap
  */
  uint32_t num = (uint32_t)(page - (struct Page *)HEAP_START) / 32;
  return (void *)((uint32_t)_alloc_start + num * PAGE_SIZE);
}

static inline int _is_free(struct Page *page) {
  if (page->flags & PAGE_TAKEN) {
    return 0;
  } else {
    return 1;
  }
}

// make the bit specified by m_flags to 1
static inline void _set_flag(struct Page *page, uint8_t m_flags) {
  page->flags |= m_flags;
}

static inline int _is_last(struct Page *page) {
  if (page->flags & PAGE_LAST) {
    return 1;
  } else {
    return 0;
  }
}

/*
  align the address to the nearest page boundary (4KiB)
  to ensure that all operations are four-byte aligned
*/
static inline uint32_t _align_page(uint32_t address) {
  /*
    order is equivalent to `4096 -1`
    0000_0000_0000_0000_0001_0000_0000_0000 - 1 =
    0000_0000_0000_0000_0000_1111_1111_1111
  */
  uint32_t order = (1 << PAGE_ORDER) - 1;
  // set the lowest 12bits to 0 and leaving the higher bits unchanged
  // make the input address rounded up to the nearest multiple of the page size
  return (address + order) & (~order);
}
static inline void _clear(struct Page *page) { page->flags = 0; }

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
  printf("\nHeap section:  start = 0x%x    SIZE:   0x%x\nMalloc Unit:   %d bit\nTotal num of pages: %d\n",HEAP_START, HEAP_SIZE, PAGE_SIZE, _num_pages);
  printf("\nALLOC:   0x%x -> 0x%x", _alloc_start, _alloc_end);
  printf("    SIZE:   0x%x\n", HEAP);
}

// allocate a memory block which is composed of contiguous physical pages
// parameter --- npages: the number of PAGE_SIZE pages to allocate
void *malloc(int bit) {
  // searching the page descriptor bitmaps
  printf("enterning malloc...\n");
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
        printf("_alloc_start: 0x%x\n", _alloc_start);
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
  void *p5 = malloc(32);
  printf("0x%x\n", p5);
  void *p6 = malloc(32);
  printf("0x%x\n", p6);
  void *p7 = malloc(32);
  printf("0x%x\n", p7);
  void *p8 = malloc(32);
  printf("0x%x\n", p8);
  void *p9 = malloc(32);
  printf("0x%x\n", p9);
  void *p0 = malloc(32);
  printf("0x%x\n", p0);
  void *p11 = malloc(32);
  printf("0x%x\n", p11);
  void *p22 = malloc(32);
  printf("0x%x\n", p22);
  void *p33 = malloc(32);
  printf("0x%x\n", p33);
  while(1){};
}

