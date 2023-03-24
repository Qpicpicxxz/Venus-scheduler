#include "task.h"

// this copy of ready actor has been fired and is deprecated
void free_ready(node_t *ready_node){
  ready_t *ready = (ready_t *)ready_node->item;
  list_t *dep_list = ready->dep_addr;
  // 1. free node of current ready fire
  delete_node(ready_node);
  free_node(ready_node);
  // 2. free ready_t struct
  free(ready);
  // 3. free current ready fire dep list
  destroy_list(dep_list);
}
  
