// Achieve dynamic memory allocation and release
#include "os.h"

extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/*
 _alloc_start --- the actual start address of heap pool
 _alloc_end --- the actual end address of heap pool
 _num_pages --- the actual max number of pages we can allocate.
*/
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096  // 4K
#define PAGE_ORDER 12

// use these macros to describe the byte's bit
#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST  (uint8_t)(1 << 1)

/* 
  Page descriptor flags:
  1. flags = 0 --- this page is allocated
  2. flags = 1 --- this page is the last page of the memory block allocated
*/
struct Page {
	uint8_t flags;
};

static inline void _clear(struct Page *page)
{
	page->flags = 0;
}

static inline int _is_free(struct Page *page)
{
	if (page->flags & PAGE_TAKEN) {
		return 0;
	} else {
		return 1;
	}
}

static inline void _set_flag(struct Page *page, uint8_t flags)
{
	page->flags |= flags;
}

static inline int _is_last(struct Page *page)
{
	if (page->flags & PAGE_LAST) {
		return 1;
	} else {
		return 0;
	}
}

// align the address to the border of page(4K)
// to ensure that all operations are four-byte aligned
static inline uint32_t _align_page(uint32_t address)
{
	uint32_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}

// we reserved 8 Page (8 x 4096) to hold the Page structures
// it should be enough to manage at most 128 MB (8 x 4096 x 4096)
// page initialze to allocate page control infomation and store blocks
void page_init()
{
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	
	struct Page *page = (struct Page *)HEAP_START;
	for (int i = 0; i < _num_pages; i++) {
		_clear(page);
		page++;	
	} 
  // heap block start address despite the page table descriptor
	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);
	uint32_t HEAP = _alloc_end - _alloc_start;
	printf("ALLOC:   0x%x -> 0x%x", _alloc_start, _alloc_end);
        printf("    SIZE:   0X%x\n", HEAP);
	printf("\nHeap section:  start = 0x%x     end = 0x%x\nTotal num of pages: %d\n", HEAP_START, HEAP_SIZE, _num_pages);
}

// allocate a memory block which is composed of contiguous physical pages
// parameter --- npages: the number of PAGE_SIZE pages to allocate
void *page_alloc(int npages)
{
  // searching the page descriptor bitmaps
	int found = 0;
	struct Page *page_i = (struct Page *)HEAP_START;
	for (int i = 0; i <= (_num_pages - npages); i++) {
		if (_is_free(page_i)) {
			found = 1;
      // encounter a free page, continue to check if the following (npages - 1) pages are also unallocated.
			struct Page *page_j = page_i + 1;
			for (int j = i + 1; j < (i + npages); j++) {
				if (!_is_free(page_j)) {
					found = 0;
					break;
				}
				page_j++;
			}
      // get a memory block which is good enough for us and address of the first page of this memory block
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
void page_free(void *p)
{
	if (!p || (uint32_t)p >= _alloc_end) {
		return;
	}
  // get the first page descriptor of this memory block
	struct Page *page = (struct Page *)HEAP_START;
	page += ((uint32_t)p - _alloc_start)/ PAGE_SIZE;
  // loop and clear all the page descriptors of the memory block
	while (!_is_free(page)) {
		if (_is_last(page)) {
			_clear(page);
			break;
		} else {
			_clear(page);
			page++;;
		}
	}
}

void page_test()
{
	void *p = page_alloc(2);
	// would print out the start address of the heap pointer
	printf("p = 0x%x\n", p);
	page_free(p);
}

