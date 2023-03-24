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

/* Function: Create a new node for linked list */
node_t* create_node(uint32_t item) {
  node_t* p = malloc(sizeof(node_t));
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
inline void free_node(node_t* p) { free(p); }

/* Function: Search item==key node in specific linkedlist */
node_t* search(list_t* list, uint32_t key) {
  node_t* p;
  for (p = list->head; p != list->tail; p = p->next)
    if (p->item == key)
      return p;
  return NULL;
}

/* Function: Insert a node follow the head */
void insert(list_t* list, node_t* p) {
  p->next = list->head->next;
  list->head->next->prev = p;
  list->head->next = p;
  p->prev = list->head;
}

/* Funciton: Delete first node */
inline void delete_node(node_t* p) {
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

/* Function: Traverse the linklist from head to tail */
void traverse(list_t* list, void (*visit)(node_t*)) {
  node_t* p;
  for (p = list->head->next; p != list->tail; p = p->next)
    visit(p);
}

/* Function: Destory a linklist */
void destroy_list(list_t* list) {
  node_t* q = list->head->next;
  node_t* p = list->head->next;
  list->head->next = list->tail;
  list->tail->prev = list->head;
  while (p != list->tail) {
    q = p;
    p = p->next;
    free_node(q);
  }
}

/* Function: delete and free a node from a list */
inline void destroy_node(node_t* node) {
  delete_node(node);
  free_node(node);
}

/* Function: Read first node (after head node) */
node_t* read_first(list_t* list) {
  if (list->tail->prev == list->head) {
    printf(RED("ERROR: Reading empty list!\n"));
    return NULL;
  } else
    return list->head->next;
}

/* Function: Read last node (before tail node) */
node_t* read_last(list_t* list) {
  if (list->tail->prev == list->head) {
    printf(RED("ERROR: Reading empty list!\n"));
    return NULL;
  } else
    return list->tail->prev;
}

/* Function: Check if it's an empty list */
uint8_t is_list_empty(list_t* list) {
  if (list->tail->prev == list->head) {
    return 1;
  } else {
    return 0;
  }
}

/* Function: Insert a new node before a specific node in a list */
void insert_before(list_t* list, node_t* node, uint32_t item) {
  /* make sure its not list head */
  assert(node != list->head);
  node_t* new = create_node(item);
  new->prev = node->prev;
  new->next = node;
  if (node == list->head) {
    list->head = new;
  } else {
    node->prev->next = new;
  }
  node->prev = new;
}

void link_test(void) {
  printf(YELLOW("\nTesting Linked List...\n"));
  node_t* node2 = create_node(2);  // 24
  printf("node 2 = 0x%x\n", node2);
  node_t* node3 = create_node(3);  // 24
  node_t* node5 = create_node(5);  // 24
  free_node(node2);
  node_t* node4 = create_node(4);  // 24
  printf("node 4 = 0x%x\n", node4);
  free_node(node3);
  free_node(node4);
  printf("node 5 = 0x%x\n", node5);
  void* p = malloc(7);
  printf("p = 0x%x\n", p);
}

