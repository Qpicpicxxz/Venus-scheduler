#include "task.h"
#include "codeAddr.h"
extern uint32_t HEAP_START;

list_t *actor_l;  /* linked list for all actor   */
list_t *ready_l;  /* linked list for ready actor */
queue_t block_q;  /* idle-block queue            */
actor_t *actor;   /* current handle actor        */
ready_t *ready;   /* current ready descriptor    */
block_f *block;   /* current block descriptor    */

/* taskCallback.c  */
extern void callback(actor_t *g);
/* block.c */
extern void block_sim(block_f *block);
extern fifo_t dma_trans_in;
/* dma.c */
extern void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len);
extern void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);
extern void free_ready(node_t *ready_node);

/* Predicate: Check whether the actor statify the fire rules */
static inline uint8_t actor_ready(void) {
  uint8_t flag = 1;
  // visit all dependencies' fifo
  for (int i = 0; i < actor->dep_num; i++) {
    flag = flag && !fifo_empty(actor->in[i]);
  }
  if (flag) {
    return 1;
  } else {
    return 0;
  }
}

/* Predicate: Check whether descendants' buffer are full */
static inline uint8_t successor_full(void) {
  uint8_t flag = 1;
  for (int i = 0; i < actor->nxt_num; i++) {
    flag = flag && fifo_full(actor->out[i]);
  }
  return flag;
}

/* Predicate: Check whether the actor could fire in this iteration */
static inline uint8_t fire_check() {
  return actor_ready() && !successor_full();
}

/* Function: A handler to bind in-flight actor with current block */
static inline void task_bind(void) {
  printf("\nBLOCK: Computing task...\n");
  block->actor = actor;
}

/* Function: Create a ready actor descriptor */
static inline ready_t *create_ready(void) {
  // 1. allocate this ready actor's descriptor space
  ready_t *r = malloc(sizeof(ready_t));
  // 2. bind the actor's address
  r->actor_addr = (uint32_t)actor;
  // 3. initialize ready actor's dependencies list
  r->dep_addr = create_list();
  // 4. traverse actor's dependencies
  for (uint32_t i = 0; i < actor->dep_num; i++) {
    // 4.1 get the dependency out from fifo
    data_t *data = get_data(actor->in[i]);
    // 4.2 bind the dependency descriptor pointer
    insert(r->dep_addr, create_node((uint32_t)data));
  }
  return r;
}

/* Function: Schedule the new ready actor to the proper position */
static inline void ready_schedule(ready_t *r) {
  int found = 0;
  for (node_t *p = ready_l->head->next; p != ready_l->tail; p = p->next) {
    int cur_nxt = ((actor_t *)r->actor_addr)->nxt_num;
    int p_nxt = ((actor_t *)((ready_t *)p->item)->actor_addr)->nxt_num;
    if (cur_nxt == p_nxt) {
      insert_before(ready_l, p, (uint32_t)r);
      found = 1;
      break;
    }
  }
  if (!found) {
    insert(ready_l, create_node((uint32_t)r));
  }
}

/* Function: Print out the current ready actor list */
static inline void print_ready_list(void) {
  printf(GREENSET "\nReady actor queue: head->");
  for (node_t *p = ready_l->head->next; p != ready_l->tail; p = p->next) {
    printf("%c->", (((ready_t *)p->item)->actor_addr - actor_start) / actor_space + 65);
  }
  printf("tail\n" RESET);
}

/* Function: Traverse all the actors in DAG to search for readys */
void ready_search(void) {
  printf(GREEN("\nChecking actor: "));
  // 1. search all the actors in DAG
  for (node_t *p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
    actor_index = ((uint32_t)p->item - actor_start) / actor_space;
    actor = (actor_t *)p->item;
    // 2. handle ready actors
    if (fire_check()) {
      // 3. schedule ready list
      ready_schedule(create_ready());
      // 4. put current actor into ready list (ready_t descriptor)
      printf(GREEN("%c✔  "), actor_index + 65);
    } else {
      printf(PINK("%c✘  "), actor_index + 65);
    }
  }
  print_ready_list();
}

/* Function: Traverse all dependencies of current and inform DMA */
static inline void traverse_data(void) {
  list_t *dep_list = (list_t *)ready->dep_addr;
  node_t *dep_node = dep_list->tail->prev;
  while(dep_node != dep_list->head) {
    // get data descriptor
    data_t *data = (data_t *)dep_node->item;
    // get current data node out of dep_addr list
    // delete_node(ready->dep_addr->tail->prev);
    // free current data node descriptor
    // free_node(ready->dep_addr->tail->prev);
    put_data(&dma_trans_in, data);
    // inform data descriptor to DMA
    dma_data(DATA1_ADDR, data->ptr, data->len);
    dep_node = dep_node->prev;
  }
  // dma inform done, free data descriptor linked list space
  // destroy(ready->dep_addr);
  // free(ready->dep_addr);
  // free this ready actor descriptor
  // free(ready);
}

/* Function: Add current fire into current actor's fire list */
static inline void add_firelist(void) {
  if (actor->fire_list == NULL)
    actor->fire_list = create_list();
  printf("SCHEDULER: Inserting block into in-flight list...\n");
  insert(actor->fire_list, create_node((uint32_t)block));
}

// NOTE: this step should invoke after DMA has already moved the code to
// blocks!!!
static inline void recycle_garbage(void) {
  printf("dep num = %d\n", actor->dep_num);
  printf("data cnt = %d\n", (read_data(&dma_trans_in))->cnt);
  for (int i = 0; i < actor->dep_num; i++) {
    data_t *data = get_data(&dma_trans_in);
      if((uint32_t)data->ptr < HEAP_START){
      	printf("free initial data , pass\n");
      	
    // check dependency's lifecycle
     } else if (data->cnt == 1) {
      // garbage collection
      printf("SCHEDULER: Last use, free data space...\n");
      free((void *)data->ptr); // free data space
      free((void *)data);      // free data flag space
    } else {
      // upate reference count (lifecycle)
      data->cnt--;
    }
  }
}

/* Function: Parse descriptor and inform DMA */
void actor_fire(void) {
  // 1. get an actor from ready actor list
  actor = (actor_t *)ready->actor_addr;
  actor_index = ((uint32_t)actor - actor_start) / actor_space;
  printf("\nSCHEDULER: Actor ");
  printf(GREEN("%c"), actor_index + 65);
  printf(" Fired\n");
  printf("SCHEDULER: 0x%x block is ready...\n", block);
  // 2. mark this block to be inflight status
  _set_block_flag(block, BLOCK_INFLIGHT);
  // 3. inform task code descriptor to DMA
  dma_code(block->spm_addr, actor->task_addr, actor->task_len);
  // 4. access each dependency in turn to inform DMA
  traverse_data();
  // 5. associate block and in-flight actor
  add_firelist();
  // 6. associate block and task
  task_bind();
  // 7. JUST SIMULATION: compute result
  block_sim(block);
  // 8. DMA get out the dependency, check their lifecycle
  recycle_garbage();
}

/* Funcition: Fire ready actors when VENUS has idle blocks */
void actor_check(void) {
  printf(GREEN("\nSCHEDULER: Waiting for blocks to be ready...\n"));
  while (1) {
    // 1. if there is any idle block and ready actor
    if (queue_size(&block_q) >= 1 && !is_list_empty(ready_l)) {
      // 2. get out this idle block from idle-queue
      block = (block_f *)get_queue(&block_q);
      // 3. parse the ready actor descriptor
      node_t *ready_node = ready_l->tail->prev;
      ready = (ready_t *)ready_node->item;
      // 4. remove this actor from ready list
      // delete_node(ready_node);
      // 5. release this ready actor node space
      // free_node(ready_node);
      // 6. invoke function with ready actor descriptor to fire actor
      actor_fire();
      //while(1){};
      // 7. actor fire done, free fired actor resource
      free_ready(ready_node);
    }
  }
}
