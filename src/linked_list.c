/* ref: http://akaedu.github.io/book/ch26s01.html */
#include "linkedlist.h"

/* Function: Create a new linkedlist */
list_t* create_list(void) {
  // 1. create a list struct
  list_t* list = malloc(sizeof(list_t));
  // 2. create head and tail sentinel node
  list->head = malloc(sizeof(node_t));
  list->tail = malloc(sizeof(node_t));
  // 3. initialize two sentinel node
  list->head->item = 0;
  list->head->prev = NULL;
  list->head->next = list->tail;
  list->tail->item = 0;
  list->tail->prev = list->head;
  list->tail->next = NULL;
  return list;
}

