#include "linkedlist.h"

/* Function: Create a new linkedlist */
list_t *create_list(void) {
  // 1. create a list struct
  list_t *list = malloc(sizeof(list_t));
  // 2. create head and tail sentinel node
  list->head = malloc(sizeof(struct node));
  list->tail = malloc(sizeof(struct node));
  // 3. initialize two sentinel node
  list->head->item = 0;
  list->head->prev = NULL;
  list->head->next = list->tail;
  list->tail->item = 0;
  list->tail->prev = list->head;
  list->tail->next = NULL;
  return list;
}

/* Function: Create a new node for linked list */
link create_node(uint32_t item) {
  link p = malloc(sizeof *p);
  p->item = item;
  p->prev = p->next = NULL;
  return p;
}

/*
 * Function: Free a node's space
 * Note: Every node could exit in only one linkedlist,
 * 	so we can just free this node, not necessary
 *	assign which linked list.
 */
void free_node(link p) { free(p); }

/* Function: Search item==key node in specific linkedlist */
link search(list_t *list, uint32_t key) {
  link p;
  for (p = list->head; p != list->tail; p = p->next)
    if (p->item == key)
      return p;
  return NULL;
}

/* Function: Insert a node follow the head */
void insert(list_t *list, link p) {
  p->next = list->head->next;
  list->head->next->prev = p;
  list->head->next = p;
  p->prev = list->head;
}

/* Funciton: Delete first node */
void delete(link p) {
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

/* Function: Transverse the linklist from head to tail */
void traverse(list_t *list, void (*visit)(link)) {
  link p;
  for (p = list->head->next; p != list->tail; p = p->next)
    visit(p);
}

/* Function: Destory a linklist */
void destroy(list_t *list) {
  link q, p = list->head->next;
  list->head->next = list->tail;
  list->tail->prev = list->head;
  while (p != list->tail) {
    q = p;
    p = p->next;
    free_node(q);
  }
}

/* Function: Push one node into linklist behind head pointer */
void push(list_t *list, link p) { insert(list, p); }

/* Function: Delete the node precedes the tail */
link pop(list_t *list) {
  if (list->tail->prev == list->head)
    return NULL;
  else {
    link p = list->tail->prev;
    delete (p);
    return p;
  }
}

/* Function: Read first node (after head node) */
link read_first(list_t *list) {
  if (list->tail->prev == list->head) {
    printf("ERROR: Reading empty list!\n");
    return NULL;
  } else
    return list->head->next;
}

/* Function: Read last node (before tail node) */
link read_last(list_t *list) {
  if (list->tail->prev == list->head) {
    printf("ERROR: Reading empty list!\n");
    return NULL;
  } else
    return list->tail->prev;
}

/* Function: Check if it's an empty list */
uint8_t is_list_empty(list_t *list) {
  if (list->tail->prev == list->head) {
    return 1;
  } else {
    return 0;
  }
}

void print_item(link p) { printf("0x%x\n", p->item); }

void link_test(void) {
  list_t *list = create_list();
  link p = create_node(0x11111111);
  insert(list, p);
  p = create_node(0x99999999);
  insert(list, p);
  p = create_node(0x77777777);
  insert(list, p);
  p = search(list, 0x99999999);
  delete (p);
  free_node(p);
  traverse(list, print_item);
  destroy(list);

  p = create_node(0x22222222);
  push(list, p);
  p = create_node(0x33333333);
  push(list, p);
  p = create_node(0x44444444);
  push(list, p);
  // suggest parentheses around assignment used as truth value
  while ((p = pop(list)) != NULL) {
    print_item(p);
    free_node(p);
  }
}

