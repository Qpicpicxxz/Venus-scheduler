### **🦆描述符结构**
---
**fifo.h**
```
typedef struct data {
  uint32_t ptr  // 数据地址指针
  uint32_t len  // 数据长度
  uint32_t cnt  // 数据生命周期(决定什么时候可以在DDR中释放这个数据内存)
} data_t
```
```
typedef struct fifo {
  data_t* data[MAXFIFO]  // fifo里的一个data_t*代表一份数据依赖
  uint8_t wptr
  uint8_t rptr
} fifo_t
```
```
typedef struct queue {
  uint32_t addr  // 简陋的fifo，用来装block的，后面可能不要了
  uint8_t wptr
  uint8_t rptr
} queue_t
```
**linkedList.h**
```
typedef struct node {
  uint32_t item
  node_t* prev
  node_t* next
} node_t   // 双向链表，拥有头、尾指针
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
  fifo_t* in[MAXIN]            // [依赖个数]
  fifo_t* out[MAXRES][MAXOUT]  // [返回值个数][后继个数]
  uint32_t task_addr           // 代码地址
  uint32_t task_len            // 代码长度
  list_t* fire_list            // 发射顺序
  list_t* linger_list          // 返回顺序
} actor_t;
```
```
typedef struct ready {
  uint32_t actor_addr          // 可以fire的actor的地址
  list_t* dep_list             // 该actor的依赖们
} ready_t
```
**block.h**
```
typedef struct block {
  uint32_t flags              // 该block的一些当前状态
  uint32_t spm_addr           // 定位该block的基地址
  actor_t* actor              // 该block正在执行的actor
} block_t
```
```
typedef struct linger {
  block_t* block              // actor本批次执行的block
  list_t* data_list           // actor本批次返回的数据(多返回值是用链表串起来的)
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
优点：最灵活，根据此task的实际性质动态添加，内存利用率高

缺点：一直在寻址，每一次`->`对应于cpu都是一次`lw`操作，查找速度相对于数组较慢
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
优点：查找快，代码书写方便

缺点：`MAXRES`和`MAXIO`是固定的，内存利用率不高

然后因为在数组中，我是需要通过`array[n] == NULL`来判断当前数组的尾部的，而`malloc()`并不保证分配的内存是干净的
也就是说我在初始化动态创建actor的时候需要清空数组，如果`MAXRES` `MAXIN` `MAXOUT`很大的话，这个开销也挺大
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
<p>优点：查找快，内存利用率高</p>
<p>缺点：需要程序员在创建actor的时候就输入此actor的入度出度情况，不能根据后续的<code>edge_make</code>函数来自动判断</p>

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

