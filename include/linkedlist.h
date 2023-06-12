/* ref: http://akaedu.github.io/book/ch26s01.html */
#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "common.h"

typedef struct node node_t;

typedef struct node {
  uint32_t item;  // a pointer or anything else
  node_t* prev;
  node_t* next;
} node_t;

typedef struct list {
  node_t* head;
  node_t* tail;
} list_t;

/* Function: Initialize a list */
list_t* create_list(void);

/* Function: Reset a list */
inline void reset_list(list_t* list) {
  list->head->next = list->tail;
  list->tail->prev = list->head;
}

/* Function: Create a new node for linked list */
inline node_t* create_node(uint32_t item) {
  node_t* p = malloc(sizeof(node_t));
  p->item   = item;
  p->prev = p->next = NULL;
  return p;
}

/* Function: Insert a node follow the head */
inline void insert(list_t* list, node_t* p) {
  p->next                = list->head->next;
  list->head->next->prev = p;
  list->head->next       = p;
  p->prev                = list->head;
}

/* Function: Check if it's an empty list */
inline uint8_t is_list_empty(list_t* list) {
  if (list->tail->prev == list->head) {
    return 1;
  } else {
    return 0;
  }
}

#endif /* __LINKEDLIST_H__ */

