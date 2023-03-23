/* ref: http://akaedu.github.io/book/ch26s01.html */
#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "os.h"
#include "types.h"


typedef struct node node_t;

typedef struct node {
  uint32_t item;	// a pointer or anything else
  node_t *prev;
  node_t *next;
} node_t;

typedef struct list {
  node_t *head;
  node_t *tail;
} list_t;

list_t *create_list(void);
node_t *create_node(uint32_t item);
void free_node(node_t *p);
node_t *search(list_t *list, uint32_t key);
void insert(list_t *list, node_t *p);
void delete_node(node_t *p);
void traverse(list_t *list, void (*visit)(node_t *));
void destroy(list_t *list);
void push(list_t *list, node_t *p);
node_t *pop(list_t *list);
node_t *read_first(list_t *list);
node_t *read_last(list_t *list);
void insert_before(list_t *list, node_t *node, uint32_t item);
uint8_t is_list_empty(list_t *list);

#endif /* __LINKEDLIST_H__ */
