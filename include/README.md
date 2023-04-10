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
} block_t
```
```
typedef struct linger {
  block_t* block
  data_t* data
} linger_t
```
---
### **actor结构体定义取舍**
---
**1. 出度和不同的返回值都串为各自的列表**
```
list_t* fifo_out_list;  // actor里面存放串联各个返回值的链表指针
```
```
list_t* result_1_list = (list_t*)fifo_out_list->head->next->item;  // 各个返回值链表串联各个返回值将要流去的fifo指针
```
```
fifo_t* token_fifo = (fifo_t*)result_1_list->head->next->item;  // 即可定位到各个fifo (DAG图的各个edge)
```
<p>好处：最灵活，根据此task的实际性质动态添加</p>
<p>坏处：代码写着很麻烦，一直在寻址</p>
</br>

**2. 出度和不同的返回值都设置成定长的数组**
```
fifo_t* fifo_out[MAXRES][MAXIO];
```
```
fifo_t* result_1 = fifo_out[0][MAXIO];
```
```
fifo_t* token_fifo = result_1[0];
```
<p>好处：查找快，代码书写方便</p>
<p>坏处：<code>MAXRES</code>和<code>MAXIO</code>是固定的，内存利用率不高</p>
</br>

  **3. 出度和不同的返回值都设置成不定长的数组**
```
fifo_t*** fifo_out;
```
```
actor_t* actor = malloc(sizeof(actor_t));
actor->out = malloc(MAXRES * sizeof(fifo_t**));
for (int i = 0; i < MAXRES; i++){
  actor->out[i] = malloc(MAXIO * sizeof(fifo_t*));
}
```
```
fifo_t* result_1 = fifo_out[0][MAXIO];
```
```
fifo_t* token_fifo = result_1[0];
```
<p>好处：查找快，内存利用率高</p>
<p>坏处：需要程序员在创建actor的时候就输入此actor的入度出度情况，不能根据后续的<code>edge_make</code>函数来自动判断</p>

***
### **👾Tips: 设计struct的时候可以考虑结构体的占用空间**
```
struct stru_1{
  uint8_t a
  uint8_t b
}
sizeof(struct stru_1) == 2
```
```
struct stru_2{
  uint8_t a
  uint32_t b
}
sizeof(struct stru_2) == 8
```
```
struct stru_3{
  uint8_t a
  uint32_t b
  uint8_t c
}
sizeof(struct stru_3) == 12
```
```
struct stru_4{
  uint8_t a
  uint8_t b
  uint32_t c
}
sizeof(struct stru_4) == 8
```
```
struct ex_4{
  uint8_t a
  uint8_t b
  uint16_t c
  uint32_t d
}
sizeof(struct ex_3) == 12
```

