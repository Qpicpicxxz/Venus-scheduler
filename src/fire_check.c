#include "hw/addressmap.h"
#include "task.h"

list_t* actor_l;       /* linked list for all actor   */
list_t* ready_l;       /* linked list for ready actor */
static actor_t* actor; /* current handle actor        */
static ready_t* ready; /* current ready descriptor    */
static block_t* block; /* current block descriptor    */

extern block_t block_stru[MAX_NUM_CLUSTERS][MAX_NUM_BLOCKS];

/* task_callback.c  */
extern void callback(actor_t* g);
/* dma/dma_lli_create.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token);
/* Predicate: Check whether the actor statify the fire rules */
static inline uint32_t actor_ready(void) {
  // visit all dependencies' fifo
  for (int i = 0; actor->in[i] != NULL; i++) {
    if (fifo_empty(actor->in[i]))
      return 0;
  }
  return 1;
}

/* Predicate: Check whether descendants' buffer are full */
static inline uint32_t successor_full(void) {
  uint8_t flag = 1;
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    for (int j = 0; actor->out[i][j] != NULL; j++)
      flag = flag && fifo_full(actor->out[i][j]);
  }
  return flag;
}

/* Predicate: Check whether the actor could fire in this iteration */
static inline uint8_t fire_check() {
  return actor_ready() && !successor_full();
}

/* Function: A handler to bind in-flight actor with current block */
static inline void task_bind(void) {
  block->actor = actor;
}

/* Function: Create a ready actor-copy descriptor */
static inline ready_t* ready_create(void) {
  // 1. allocate this ready actor's descriptor space
  ready_t* actor_r = malloc(sizeof(ready_t));
  // 2. bind the actor's address
  actor_r->actor_addr = (uint32_t)actor;
  // 3. initialize ready actor's dependencies list
  actor_r->dep_list = create_list();
  // 4. traverse actor's dependencies
  for (int i = 0; actor->in[i] != NULL; i++) {
    // 4.1 get the dependency out from fifo
    token_t* token = get_token(actor->in[i]);
    // 1. JUDGE whether it's a vector or scalar
    // TODO: if its a scalar, then channel index represent the scalar array offset
    if (READY_CREATE_IS_SCALAR(token->attr)) {
      token->attr = i;
    } else {
      // TODO: if its a vector, then in line with the result length and bind information (which vreg), to assign proper vreg-lut address
    }
    // 4.2 bind the dependency descriptor pointer
    insert(actor_r->dep_list, create_node((uint32_t)token));
  }
  return actor_r;
}

/* Function: Schedule the new ready actor to the proper position */
static inline void ready_insert(ready_t* r) {
  // for now, first ready first serve
  insert(ready_l, create_node((uint32_t)r));
}

/* Function: Traverse all the actors in DAG to search for readys */
void ready_search(void) {
  // 1. search all the actors in DAG
  for (node_t* p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
    actor = (actor_t*)p->item;
    // 2. handle ready actors
    while (fire_check()) {
      // 3. schedule ready list
      ready_insert(ready_create());
    }
  }
}

/* Function: For now by default, first ready first fire */
static inline node_t* ready_select() {
  // TODO: when found an affinity actor, then do not move the code to block ispm again
  // uint32_t last_actor = (uint32_t)block->actor;
  // for (node_t* p = ready_l->tail->prev; p != ready_l->head; p = p->prev) {
  //   uint32_t cur_actor = ((ready_t*)p->item)->actor_addr;
  //   // if found an affinity actor
  //   if (cur_actor == last_actor) {
  //     return p;
  //   }
  // }
  // if there is no affinity actor, return the tail of the ready list
  return ready_l->tail->prev;
}

/* Function: Free the fired/deprected ready actor-copy */
static inline void ready_free(node_t* ready_node) {
  ready_t* ready   = (ready_t*)ready_node->item;
  list_t* dep_list = ready->dep_list;
  // 1. free node of current ready fire
  destroy_node(ready_node);
  // 2. free current ready fire dep list
  destroy_list(dep_list);
  // 3. free ready_t struct
  free(ready);
}

/* Function: Traverse all dependencies of current and inform DMA */
static inline void inform_dma(void) {
  token_t* pseudo_token = NULL;
  block_t* pseudo_block = NULL;
  dma_transfer_link(block->base_addr + BLOCK_ISPM_OFFSET, actor->task_addr, actor->task_len, pseudo_block, pseudo_token);
  list_t* dep_list = (list_t*)ready->dep_list;
  for (node_t* p = dep_list->tail->prev; p != dep_list->head; p = p->prev) {
    token_t* token = (token_t*)p->item;
    // inform data descriptor to DMA
    if (p == dep_list->head->next) {
      if (INFORM_DMA_IS_VECTOR(token->attr)) {
        // TODO: look up the Vector Register table
      } else {
        // this data is a scalar
        uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + token->attr * SCALAR_LEN;
        dma_transfer_link(scalar_offset, token->data->ptr, SCALAR_LEN, block, token);
      }
    } else {
      if (INFORM_DMA_IS_VECTOR(token->attr)) {
        // TODO: look up the Vector Register table
      } else {
        // this data is a scalar
        uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + token->attr * SCALAR_LEN;
        dma_transfer_link(scalar_offset, token->data->ptr, SCALAR_LEN, pseudo_block, token);
      }
    }
  }
}

/* Function: Add current fire into current actor's fire list */
static inline void add_firelist(void) {
  if (actor->fire_list == NULL)
    actor->fire_list = create_list();
  insert(actor->fire_list, create_node((uint32_t)block));
}

/* Function: Parse descriptor and inform DMA */
static inline void actor_fire(void) {
  // 1. mark this block to be inflight status
  _set_block_flag(block, BLOCK_INFLIGHT);
  // 2. associate block and in-flight actor
  add_firelist();
  // 3. associate block and task
  task_bind();
  // 4. inform task code and dependency to DMA
  inform_dma();
}

/* Function: Find an idle block */
uint32_t block_idle(void) {
  for (int i = 0; i < NUM_CLUSTERS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      if ((block_stru[i][j].flags & BLOCK_INFLIGHT) == 0) {
        block = &block_stru[i][j];
        return 1;
      }
    }
  }
  return 0;
}

/* Funcition: Fire ready actors when VENUS has idle blocks */
void actor_check(void) {
  while (1) {
    // 1. if there is any idle block and ready actor
    if (block_idle() && !is_list_empty(ready_l)) {
      // 3. parse the ready actor descriptor
      node_t* ready_node = ready_select();
      ready              = (ready_t*)ready_node->item;
      actor              = (actor_t*)ready->actor_addr;
      // 4. fire actor
      actor_fire();
      // 5. actor fire done, free fired actor resource
      ready_free(ready_node);
    }
  }
}

