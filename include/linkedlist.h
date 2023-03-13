#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "os.h"
#include "types.h"


typedef struct node *link;

typedef struct node {
  uint32_t item;	// a pointer or anything else
  link prev;
  link next;
} node_t;

typedef struct list {
  link head;
  link tail;
} list_t;

list_t *create_list(void);
link create_node(uint32_t item);
void free_node(link p);
link search(list_t *list, uint32_t key);
void insert(list_t *list, link p);
void delete(link p);
void traverse(list_t *list, void (*visit)(link));
void destroy(list_t *list);
void push(list_t *list, link p);
link pop(list_t *list);
link read_first(list_t *list);
link read_last(list_t *list);
uint8_t is_list_empty(list_t *list);

#endif /* __LINKEDLIST_H__ */
