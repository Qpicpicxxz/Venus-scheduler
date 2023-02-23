#include "page.h"

#define BLOCK_SIZE 48
#define NODE_SIZE 16
#define BLOCK_END PAGE_SIZE

struct Node {
  uint16_t next;
};

struct Block {
  uint16_t pre;
  uint16_t size;
  uint16_t next;
};

static inline void set_pre(struct Block *b, uint16_t pre) { b->pre = pre; }
static inline void set_next(struct Block *b, uint16_t next) { b->next = next; }
static inline void set_size(struct Block *b, uint16_t size) { b->size = size; }

struct Page *find_page(struct Page *pre, int *cnt) {
  struct Page *ret = NULL;
  for (;;) {
    ++pre;
    (*cnt)++;
    if (*cnt >= _num_pages) {
      break;
    }
    if (!_is_free(pre)) {
      ret = pre;
      break;
    }
  }
  return ret;
}

int size_check(int size) {
  if (size <= 0)
    return 0;
  return 1;
}

int ptr_check(void *p) {
  if (!p || ((uint32_t)p >= _alloc_end)) {
    return 0;
  }
  return 1;
}
void *new_block(int size) {
  void *ret = page_alloc(1);
  if (!ret)
    return ret;

  uint16_t start = NODE_SIZE;

  struct Node *node = (struct Node *)ret;
  node->next = start;

  struct Block *st = (struct Block *)((uint32_t)ret + start);
  set_pre(st, 0);
  set_size(st, size);
  set_next(st, BLOCK_END);

  return (void *)((uint32_t)ret + start + BLOCK_SIZE);
}
void show_block(struct Block *b) {
  printf("address\t: 0x%x\npre\t: 0x%x\nnext\t: 0x%x\nsize\t: 0x%x\n", b,
         b->pre, b->next, b->size);
}
static inline uint32_t distance(struct Block *b, struct Node *base) {
  uint32_t offset = (uint32_t)b - (uint32_t)base;
  return b->next - b->size - BLOCK_SIZE - offset;
}
static inline uint16_t rdistance(struct Block *b, struct Node *base) {
  return (uint32_t)b - (uint32_t)base;
}
void *insert_block(struct Page *page, int size) {
  struct Node *node = (struct Block *)_get_addr(page);
  int nsize = BLOCK_SIZE + size;
  uint32_t pt = node->next;
  while (1) {
    struct Block *b = (struct Block *)((uint32_t)node + pt);
    show_block(b);
    if (distance(b, node) >= nsize) {
      struct Block *ret = (struct Block *)((uint32_t)b + b->size + BLOCK_SIZE);
      uint16_t addr = (uint32_t)ret - (uint32_t)node;

      set_next(ret, b->next);
      set_pre(ret, rdistance(b, node));
      set_size(ret, size);

      if (b->next < BLOCK_END) {
        struct Block *bnext = (struct Block *)((uint32_t)node + b->next);
        set_pre(bnext, addr);
      }
      set_next(b, addr);

      return (void *)((uint32_t)ret + BLOCK_SIZE);
    }
    pt = b->next;
    if (pt >= BLOCK_END) {
      break;
    }
  }
  return NULL;
}

void *malloc(int size) {
  // there is not enough memory space
  if (!size_check(size))
    return NULL;
  // *page pointer will point to a Page object(located immediately before the address `HEAP_START`
  struct Page *page = (struct Page *)HEAP_START - 1;
  int cnt = -1;
  if (size >= PAGE_SIZE - BLOCK_SIZE - 16) {
    int num = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    void *ret = page_alloc(num);
    page = _get_page(ret);
    _set_flag(page, HUGE_BLOCK);
    return ret;
  }

  while (1) {
    page = find_page(page, &cnt);
    if (!page)
      break;
    void *ret = insert_block(page, size);
    if (ret)
      return ret;
  }

  return new_block(size);
}

void free(void *ptr) {
  if (!ptr_check(ptr)) {
    return;
  }

  struct Page *page = _get_page(ptr);

  if (page->flags & HUGE_BLOCK) {
    page_free(page);
    return;
  }

  uint32_t node = (uint32_t)_get_addr(page);

  struct Block *b = (struct Block *)((uint32_t)ptr - BLOCK_SIZE);
  if (b->pre == 0) {
    struct Node *pre = (struct Node *)node;
    pre->next = b->next;
  } else {
    struct Block *pre = (struct Block *)(node + b->pre);
    set_next(pre, b->next);
  }

  if (b->next < BLOCK_END) {
    struct Block *post = (struct Block *)(node + b->next);
    set_pre(post, b->pre);
  }

  if (((struct Node *)node)->next == BLOCK_END) {
    page_free((void *)node);
  }
}

void malloc_test() {
  void *p1 = malloc(1);
  void free(p1);
  void *p2 = malloc(1);
  void *p3 = malloc(1);
  printf("p1 = 0x%x\n", p1);
  printf("p2 = 0x%x\n", p2);
  printf("p3 = 0x%x\n", p3);
}

