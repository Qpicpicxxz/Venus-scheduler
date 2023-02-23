// Achieve dynamic memory allocation and release

#include "page.h"

/* 
   we reserved 8 Page (8 x 4096) to hold the Page structures
   it should be enough to manage at most 128 MB (8 x 4096 x 4096)
   page initialze to allocate page control infomation and store blocks
*/
void page_init()
{
	// the first 8 pages can be used for metadata
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	
	struct Page *page = (struct Page *)HEAP_START;
	for (uint32_t i = 0; i < _num_pages; i++) {
		_clear(page);
		page++;	
	} 
  	// heap block start address despite the page table descriptor
  	// _alloc_start aligned to the beginnning of the 9th page
	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);
	uint32_t HEAP = _alloc_end - _alloc_start;
	printf("\nHeap section:  start = 0x%x    SIZE:   0x%x\nTotal num of pages: %d\n", HEAP_START, HEAP_SIZE, _num_pages);
	printf("\nALLOC:   0x%x -> 0x%x", _alloc_start, _alloc_end);
        printf("    SIZE:   0x%x\n", HEAP);
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
                       // encounter a free page,
                       // continue to check if the following (npages - 1) pages are also unallocated.
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
void page_free(void *ptr)
{
	if (!ptr || (uint32_t)ptr  >= _alloc_end) {
		return;
	}
  // get the first page descriptor of this memory block
	struct Page *page = (struct Page *)HEAP_START;
	page += ((uint32_t)ptr - _alloc_start)/ PAGE_SIZE;
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

