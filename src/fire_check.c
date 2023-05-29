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
  // if all the dependencies prepared
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
// static inline ready_t* ready_create(void) {
//   ready_t* actor_r = malloc(sizeof(ready_t));
//   actor_r->actor_addr = (uint32_t)actor;
//   actor_r->dep_list = create_list();
//   for (int i = 0; actor->in[i] != NULL; i++) {
//     token_t* token = get_token(actor->in[i]);
//     insert(actor_r->dep_list, create_node((uint32_t)token));
//   }
//   return actor_r;
// }

/* Function: Create a ready actor-copy descriptor */
static inline ready_t* ready_create(void) {
  // check if its a pseudo task
  if (read_token(actor->in[0]) == (token_t*)PSEUDO_TOKEN_LABEL) {
    // if it's a pseudo task, then get out all it's dependencies and do return a NULL ready_actor struct
    for (int i = 0; actor->in[i] != NULL; i++) {
      get_token(actor->in[i]);
    }
    return NULL;
  }

  // allocate this ready actor's descriptor space
  ready_t* actor_r = malloc(sizeof(ready_t));
  // bind the actor's address
  actor_r->actor_addr = (uint32_t)actor;
  // initialize ready actor's dependencies list
  actor_r->dep_list = create_list();
  for (int i = 0; actor->in[i] != NULL; i++) {
    // get the dependency out from fifo
    token_t* token        = get_token(actor->in[i]);
    token_t* pseudo_token = (token_t*)PSEUDO_TOKEN_LABEL;
    // if its a real dependency
    if (token != pseudo_token) {
      // judge whether it's a vector or scalar
      // TODO: if its a scalar, then channel index represent the scalar array offset
      if (READY_CREATE_IS_SCALAR(token->attr)) {
        // TODO: change attr into scalar offset (VENUS branch has done this job)
        token->attr = i;
      } else {
        // TODO: if its a vector, then in line with the result length and bind information (which vreg), to assign proper vreg-lut address
      }
      // bind the dependency descriptor pointer
      insert(actor_r->dep_list, create_node((uint32_t)token));
    }
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
  // search all the actors in DAG
  for (node_t* p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
    actor = (actor_t*)p->item;
    // handle ready actors
    while (fire_check()) {
      // // 3. schedule ready list
      // ready_insert(ready_create());
      // schedule ready list, ready_create() step will get out the token
      ready_t* ready_actor = ready_create();
      if (ready_actor) {
        ready_insert(ready_actor);
      } else {
        // pass pseudo token to it's successor, skip block calculation step
        for (int i = 0; actor->out[i][0] != NULL; i++) {
          for (int j = 0; actor->out[i][j] != NULL; j++) {
            // create a pseudo_token
            token_t* pseudo_token = (token_t*)PSEUDO_TOKEN_LABEL;
            put_token(actor->out[i][j], pseudo_token);
          }
        }
      }
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
  // return ready_l->head->next;
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
  // Start of the scalar spm address
  uint32_t offset = 0;
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
        uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + offset;
        dma_transfer_link(scalar_offset, token->data->ptr, token->attr, block, token);
      }
    } else {
      if (INFORM_DMA_IS_VECTOR(token->attr)) {
        // TODO: look up the Vector Register table
      } else {
        // this data is a scalar
        uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + offset;
        dma_transfer_link(scalar_offset, token->data->ptr, token->attr, pseudo_block, token);
      }
    }
    // update current offset
    offset += token->attr;
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

