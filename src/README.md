### **🦆描述符结构**
***
**fifo.h**
```
typedef struct data {
  uint32_t ptr
  uint32_t len
  uint32_t cnt
} data_t
```
```
typedef struct fifo {
  data_t* data[MAXFIFO]
  uint32_t ptr
} fifo_t
```
```
typedef struct queue {
  uint32_t addr
  uint32_t ptr
} queue_t
```
**linkedList.h**
```
typedef struct node {
  uint32_t item
  node_t* prev
  node_t* next
} node_t
```
```
typedef struct list {
  node_t* head
  node_t* tail
} list_t
```
**actor.h**
```
typedef struct actor {
  fifo_t* in[MAXIO]
  uint32_t dep_num
  fifo_t* out[MAXIO]
  uint32_t nxt_num
  uint32_t result_len
  uint32_t task_addr
  uint32_t task_len
  list_t* fire_list
  list_t* linger_list
} actor_t
```
```
typedef struct ready {
  uint32_t actor_addr
  list_t* dep_list
} ready_t
```
**block.h**
```
typedef struct block {
  uint32_t flags
  uint32_t spm_addr
  actor_t* actor
  uint32_t result
} block_t
```
```
typedef struct linger {
  block_t* block
  data_t* data
} linger_t
```
### **👻整个task里面通用的变量**
***
**actorInit.c**
```
actor_index
actor_space
actor_start
```
**fireCheck.c**
```
actor_l  // DAG里所有的actor描述符链表
ready_l  // 能够发射的所有ready actor描述符链表
block_q  // block描述符队列(一开始就初始化好了，静态的)
```
***
```
static actor_t* actor
static ready_t* ready
static block_t*block
```
```
ready_create(){ // 首先创建一个ready actor的描述符，把数据从fifo中取出来[L2]
  allocate space for ready_t *actor_r
  actor_r->addr = actor // 这个是在上层遍历actor list的时候赋值的
  actor_r->dep_list = create_list
  traverse actor dependences{
  get data from dependences' fifo(在这里就把数据流中的数据取走了)
  insert(actor_r->dep_list, create_node(data))
  }
  return actor_r
}

```
```
ready_search(){
  traverse actor list{
    if(fire_check()){  // 在所有actor的遍历中找到了满足发射条件的actor
        ============================================================
        ready_insert(ready_create()){
        从ready_create()拿到ready, 进行schedule,
        这里可以根据自定义的actor发射优先级或者其它block无关的东西进行调度
        目前的实现:
          traverse ready list from list tail{ // 从尾部开始遍历(block会默认从尾部开始取actor[L1]，所以ready list尾部的那个actor copy优先级是最高的)
          以下是目前的算法：
          cur_nxt = 当前actor的出度
          p_nxt = 当前遍历到的ready list里面的acotr的出度
          出度作为优先级
        }
      }
      ==============================================================
    }
  }
}
```
```
ready_free(node_t *ready_node){[L2]
  ready = reade_node->item
  dep_list = ready->dep_list
  destroy_node(ready_node) // 将这个node从原有链表删除，并释放其动态分配的描述符空间
  desroy_list(dep_list)  // 释放该list上所有的node描述符空间以及这个list本身自己的描述符空间
  free(ready) // 释放该ready的描述符空间
}
```
```
ready_select(){
  ======================================================
  // 可以根据当前block来选择要哪个(此处需要一定的调度选择)
  // 比如根据当前中断的block的上一个执行的任务的actor，来选择要不要找一个一样的actor发射，节省code传输时间
     如果这样的话
     // TODO: 需要有一个标记来表示这次DMA不需要传输code
  ...
  ======================================================
  return (node_t*)ready_node
}
```
```
actor_check(){
  while(1){
    if (block queue is not empty){
      block = current block
      // 默认直接从ready list的最末尾拿一个actor[L1]
      // ready = ready_l->tail->prev->item (deprecated)
      ready_node = ready_select()
      ready = ready_node->item
      (在这里可以根据中断block的实际情况，来选择到底后面发射ready list里面的哪一个actor copy)
      actor = ready->actor_addr
      actor_fire(){
        _set_block_flag(block, BLOCK_INFLIGHT)
        inform_dma(){
          dma_code()
          traverse ready->dep_list(){
            dma_data()
          }
        }
        add_firelist();
        task_bind(){
          block->actor = actor
        }
        block_sim(){
          block->result = result
        }
        recycle_garbage(){
          check data lifecycle
        }
      }
      free_ready(ready_l->tail->prev)
    }
  }
}
```
**🕊️taskCallback.c**
```
ideal_block
result
cur_actor
cur_data
```
```
pass_result(){
  traverse cur_actor->data and pass to the successor
  free current fire node and linger node
  if (fire_list is not empty){
    ideal_block = tail of the fire list
  }
}
```
```
block_recycle(n_block){
  result = n_block->result
  if (block is INFLIGHT){
    linger_insert()
    cur_actor = n_block->actor
    alloc_result(){
      assign cur_data
      dma_result()
      ideal_block = tail of fire list (最早发射的)
      actual_block = head of linger list (刚传回来的)
      if (ideal_block != actual_block){
        linger->data = cur_data (暂时保存disorder的结果指针，便于后面传)
      } else {
        pass_result()
        while(linger list is not empty){
          traverse linger list{
            if (linger->block = ideal block){
              cur_data = linger->data
              pass_result()
            }
          }
        }
      }
    }
    _clear_block_flag(n_block);
    ready_search(); (在fireCheck里面)
  }
  if (block is not in BLOCKFIFO){
    _set_block_flag(n_block, BLOCK_INFIFO);
    add current block into idle block queue
  }
}
```
***
