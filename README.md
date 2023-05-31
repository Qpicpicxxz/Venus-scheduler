# Venus-scheduler

### This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband

**Tips**

1. 在软件中暂停vcs仿真：`printf("anything u want to console $stop\n")`

2. scheduler程序起始地址：`.PROGADDR_RESET (32'h 8000_0000)`

3. irq vector地址：`.PROGADDR_IRQ (32'h 8000_0020)`

4. venus自定义的irq是level triggered：`.LATCHED_IRQ (32'h 0000_0007)`

5. 定义一个task的步骤：

   - 在`./task/code`目录下创建文件`taskname.c`

   - 在`./task/dag.c`中通过`actor_t* A  = actor_create(TASKNAME_START, TASKNAME_LEN)`来通知调度器创建此actor（文件名全大写+_START 和 _LEN）

   - 在`./task/dag.c`中通过`edge_make(A, m, B, n)`来表述具体的依赖关系，其中`m`表示对应于actorA的第几个返回值，`n`表示对应于actorB的第几个依赖

     - 表示第`m`个返回值：` WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(m), (uint32_t)retaddr0);`

     - 表示第`n`个依赖：`input_data[n]`

   - 在`./task/dag.c`中通过`assign_root(actor)`和`assign_sink(actor)`来指定DAG图中的根节点与尾部节点

6. pico不支持中断嵌套，需要等待中断发生的时候要记得返回上一个中断，或者`picorv32_waitirq_insn(irq)`(在汇编里才能用)

7. 调度器使用的堆管理方法（`malloc`等）是使用的**explicit free list**，即显式空闲列表块，这里的每一个free block里面都显示的指明了下一个空闲块和上一个空闲块。这样做的好处就是在`malloc`的时候不需要遍历所有的block，只需要遍历free block即可。所以在`mem_block.c`中需要维护一个free block list的表头和个数，这两个是全局变量，在`malloc`和`free`的时候会进行修改。但是这样会出现一个问题，就是若当调度器普通的执行流在分配堆内存的时候，若外部产生了一个中断，而中断回调函数中也有分配堆内存的操作，这时就会产生全局变量紊乱。所以我在`malloc`和`free`的时候使用了一个`Mask_irq`函数，对应于PicoRV32自定义汇编的`picorv32_maskirq_insn(rd,rs)`指令，在函数执行前暂时禁用所有外部中断，函数返回前开启中断。

8. 对于block的`malloc`函数，或许可以以后在评估性能的时候决定是否需要简化。因为越复杂的`malloc`代表其代码量越大，执行时间可能越长，内存分配效率越高。但是我们block上面的堆空间或许也不需要追求这么高的分配效率，可以取一个折中，重新设计一个`malloc`函数。

9. 启动某些block的uart输出功能:`../src/rtl/venus_soc_pkg.sv`里的`VENUS_BLOCK_DEBUG_UART_AVAILABLE_LIST`以及`VENUS_BLOCK_UART_PRINT_TO_VCS_CONSOLE_AND_FILE_AVAILABLE_LIST`配置

10. 启动部分block：在`src/init.c`中设置`devctrl_init()`函数，`VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER`32bit的后16位表示开启(硬复位)相应的block

11. scheduler上电的时候是没有对heap初始化的，所以一切malloc出来的结构体若不直接赋值，则需要进行初始化，而不能直接判断`xxx == NULL`

12. block软复位的时候不保证未初始化的全局变量为0(为了提高执行效率，**Soft Reset**的时候并不会对.bss段进行赋0操作)，并且与上面同理，malloc的内存不仅不保证干净，并且还有可能未初始化(直接读就会产生读出`0xxxxxxxxx`的情况)，若有需要可以赋值`NULL` 或者`0`或通过`WRITE_BURST_32()`来向内存先写一个初始值。

13. 调度步骤：

    - 调度器上电`actor_init.c`，动态创建actor以及fifo的描述符，每两个actor之间的依赖关系都用一个特定的fifo表示（也就是DAG图中有多少线就有多少个fifo），并将所有的actor串在`actor_l`的链表上。

    - 调度器轮询任务`fire_check.c`，首先执行一次`ready_search()`，将满足发射条件的actor串在`ready_l`的链表上。调度器初始化完毕后，跳转至`actor_check()`循环，当有空闲的block以及`ready_l`不为空的时候，进行actor的发射。

      - 其中发射之前调度器会将此actor的任务发射顺序（比如首先是block 0_00发射了这个actor，然后block 1_01发射了这个actor）记录在actor的描述符中，后续可以判断block返回是否乱序。

      - 调度器会在这个阶段将当前block的flag标注为`BLOCK_INFLIGHT`，将此block标注为忙碌。

      - 通知DMA`dma/dma_lli_create.c - dma_transfer_link()`，此时调度器可以从`token->attr`中知道当前数据搬移的目的偏移地址，调度器会将task code以及所有dependencies创建的LLI串在一起，统一交给一路DMA Channel进行传输。

    - DMA拉高中断`dma/dma_transfer_done.c - dma_transmit_done_handler()`，首先调度器会根据当前Channel的index判断是哪一批传输完成了`msg.h`，接着释放分配的LLI空间，判断block的flag（`BLOCK_RESULT`），来确定是返回值传输完成了还是任务发射准备好了。

      - 前者将会将返回值输入后续的fifo，并清空block的flag，接着调用一次`ready_search()`，来将新的满足发射条件的actor串进`ready_l`。

      - 后者将会打开相应block的soft reset，启动block的软复位，进行计算工作。

    - block拉高中断`task_callback.c - block_handler()`，首先会将block的flag设置为`BLOCK_RESULT`，然后读取block的Control Registers来知晓返回值具体信息（个数、长度），接着将所有数据的LLI串起来通知一路DMA Channel进行传输。

14. block产生中断的时候判断其状态：

    ```c
    // 判断当前block是否空闲 - ACTOR FIRE的时候置位 - ACTOR CHECK的时候判断
    #define BLOCK_INFLIGHT (uint32_t)(1 << 0)
    // 判断当前block是否需要被回收数据 - BLOCK中断的时候置位 - DMA中断的时候判断/复位
    #define BLOCK_RESULT (uint32_t)(1 << 1)
    ```

    没有选择通过硬件的**soft reset**寄存器位来读取block是否空闲的原因：因为根据调度器的逻辑，我在判断到有ready actor以及idle block的时候就已经选择了一个block进行发射，但此时因为DMA还未传输相应的执行数据代码，所以并不能将block的**soft reset**打开（需要等DMA传输完成后的回调打开），但是我调度器通知DMA传输数据和代码之后就代表此actor已经fire了，会返回到主循环进行下一次判断，所以调度器需要自己设置一个标志位来判断block是否空闲

15. 编译出来的`os.bin`大小必须是8byte的倍数，因为venus的硬件的bootroom的最小存储单元是8byte，如果没有8byte-aligned，则会导致未对齐部分被忽略，因为scheduler的`os.bin`是以某个task的.bin板块结尾的(在`os.ld`中，最后一个`.taskN`段后面接壤`.bss`段，但在.bin中会被省略)，如果某部分task的.bin文件不齐全，则会导致执行了`DMAC_CHx_enable_channel`函数后DMA卡住(不产生interrupt也不释放该channel)

解决方案：在os.bin结尾处(.bss之前)写一些冗余数据

```c
# python end jacklight

    .padding :{
        *(.padding)
    } >ram

    .bss :{
        PROVIDE(_bss_start = .);
        *(.bss.stack)
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    } >ram
```

```c
unsigned int padding_bin[] __attribute__((section(".padding"))) = {0xffff,0xffff};
```

16. 根据`objdump.txt`和`dve_gui.log`查看程序执行情况：

比如我想看`edge_make`中是生成了哪个fifo

```c
void edge_make(actor_t* src, uint8_t dep_index, actor_t* snk, uint8_t snk_index) {
  assert(dep_index < MAXRES);
  fifo_t* fifo = malloc(sizeof(fifo_t));
  init_fifo(fifo);
  int success;
  success = node_make_out(src->out[dep_index], fifo);
  if (!success) {
    printf("Dependency actor's fifo is full\n");
    printf("Actor: %p -> %p $stop\n", src, snk);
  }
  node_make_in(snk->in, fifo, snk_index);
}
```

首先`objdump.txt`中搜到`edge_make`函数，找到其反汇编，函数返回参数为`a0`，所以直接看`malloc`返回后的第一个`a0`的值，在`dve_gui.log`中搜索`80003ec0`

```
80003e40 <edge_make>:
80003e40:    fd010113              addi    sp,sp,-48
...
80003eb4:    02c00513              li    a0,44
80003eb8:    e85fd0ef              jal    ra,80001d3c <malloc>
80003ebc:    fea42623              sw    a0,-20(s0)
80003ec0:    fec42503              lw    a0,-20(s0)  // 最好搜这条
80003ec4:    975ff0ef              jal    ra,80003838 <init_fifo>
80003ec8:    fdb44783              lbu    a5,-37(s0)
...
80003f38:    02812403              lw    s0,40(sp)
80003f3c:    03010113              addi    sp,sp,48
80003f40:    00008067              ret
```

找到最近的一个write

```
write  0x800104e4: 0x80011180 (wstrb=1111)
```

则该`malloc`的结果为`0x80011180`





17. 调整搬运到DDR上的代码的大小：`./src/testbench/test_venus_scheduler_core.sv`里面修改81行for循环语句的`i`值



**区分scalar和vector**

数据流模型的主体虽然是数据，但是若**fifo**中只流动着各个数据对应的指针，那么调度器无从知晓这些数据的流向以及状态。目前data涉及两个比较重要的属性：

1. 生命周期`data->cnt`：因为每一次中间数据只会在内存中分配一次，但是可能会被多个后继使用。因为我们的内存不是无限大的，中间数据是需要动态释放的，所以我们需要一个指示来判断该数据是否已经可以被free掉（也就是所有的后继都把它使用了），**这个指示是需要对应于一个data的pointer**。

2. 数据属性`token->attr`：因为调度器在通知DMA搬运数据的时候，需要知道数据的搬运目的地，而这个address是不同的actor有不同的spm目的地址（对于同一个中间数据来说），所以这个属性需要每一个fifo独有一份（一个fifo表示某两个actor的某一路依赖）。所以这个数据到底要搬运到哪里，是需要

   - 其中，在block中断返回的时候（`task_callback.c` - `alloc_result()`），会去读取block的Control Registers，这时可以知道返回值的具体信息（scalar / vector / length），此时会赋值一次`token->attr`，表明从block那里获取到的token信息。

   - 在创建满足发射条件的actor`ready_create()`的时候，会根据当前actor的信息来决定具体的`token->attr`的值。

```c
/* data descriptor */
typedef struct data {
  uint32_t ptr;  // where the data resides
  uint32_t cnt;  // lifecycle
} data_t;        /* data template */
```

```c
/* token descriptor */
typedef struct token {
  data_t* data;   // different token struct may contain same data
  uint32_t attr;  // If it's a scalar, then attr means array index. If it's a vector, then attr means vector register list address
} token_t;        /* token template */
```

```c
/* fire_check.c - inform_dma() - 用来判断是否是vector data */
#define INFORM_DMA_IS_VECTOR(x) ((x)&0x80000000)

/* task_callback.c - alloc_result() - 在读取block的Ret寄存器的时候用来标注是否是标量 */
#define SCALAR_LABEL              0

/* fire_check.c - ready_create() - 用来判断是否是scalar data */
#define READY_CREATE_IS_SCALAR(x) (x == SCALAR_LABEL)
#define SCALAR_LEN                4

/* 存储在DDR中的一个表，我可以根据这个每个标号地址来判断这是个什么样子的向量 (还没写)
 * +-----+
 * | V10 |
 * | V12 |
 * | V13 |
 *   ...
 * | Vnn |
 * +-----+
 */
 ···
```
**当前版本的Timepoint**

**PicoRV32执行指令条数与运行时间**
```
time: 2000052000ps index: 28920
time: 3000062000ps index: 42990  -------- 14.07 us/instr
time: 4000022000ps index: 58122  -------- 15.13 us/instr
time: 5000012000ps index: 73157  -------- 15.04 us/instr
time: 6000012000ps index: 88269  -------- 15.11 us/instr
time: 7000045000ps index: 103395 -------- 15.13 us/instr
time: 8000081000ps index: 118546 -------- 15.15 us/instr
time: 9000064000ps index: 133568 -------- 15.04 us/instr
```


**Scheduler上电执行流**
```
800002d0 <_start> [123943000, 1]
- zero all registers
- initialize .bss section
- initialize stack pointer
800003f8 <start_kernel> [332913000, 2957]
  80000a40 <devctrl_init> [333216000, 2962]
    - enable AHB & DMA & DEBUG UART & CLUSTERS + BLOCKS
  80001960 <irq_init> [335855000, 2989]
    80001674 <set_handler> × 8 [336288000, 2997]
      - set DMA & CLUSTERS & EBREAK irq handlers
    800016b4 <enable_irq> × 10 [346727000, 3175]
      - enable necessary interrupt
  800057ec <dma_init> [363300000, 3476]
    - DMAC reset & config
  80001ba8 <heap_init> [383196000, 3815]
    80001a6c <init_sentinel> [386871000, 3870]
      - initialize heap space's border
    8000283c <free_list_insert> [419881000, 4390]
      - initialize free list
  80000900 <block_struct_init> [423720000, 4449]
    - initialize block descriptor's information
  80000844 <msg_array_init> [858302000, 12575]
    - bind DMA channel index with msg descriptor
  80003af8 <actor_launch> [860821000, 12623]
    80003a8c <list_init> [861125000, 12628]
      80002a7c <create_list> [861428000, 12633]
        - create all actor's list
      80002a7c <create_list> [1028564000, 15157]
        - create ready actor's list
    800065a4 <DAG_depict> [1196007000, 17686]
      80006464 <dag_create> [1196310000, 17691]
        80003b2c <actor_create> × 4
          - 6300 instr/actor_create
        80003d34 <edge_make> × 7
          - 940 instr/edge_make
        80003ee8 <assign_root> [3437570000, 49601]
        80003f30 <assign_sink> [3494441000, 50469]
    80003ad0 <schedule_init> [3725562000, 53964]
      800033cc <ready_search> [3725865000, 53969]
        - traverse all actors[
          - if fire check is true
            - if its a real task
              80003274 <ready_create> [3743784000, 54255]
                - get out all ready actor's dependencies from its input FIFO and add it into actor's dep_list
              80003378 <ready_insert> [4031453000, 58594]
                - insert current ready actor's descriptor into ready list node
            - if its a pseudo task
              - pass pseudo token into successor
        ]
    8000380c <actor_check>  [4110168000, 59800]
      while(1)[
        80003718 <block_idle> [4110471000, 59805]
          - traverse all enabled block[
            - if there is an idle block then bind this block's descriptor into current file's global block descriptor
          ]
        80002f18 <is_list_empty> [4113693000, 59865]
          - check if ready list is empty
        80003460 <ready_select> [4114849000, 59883]
          - select a ready actor to fire
        800036d4 <actor_fire> [4116392000, 59907]
          8000303c <_set_block_flag> [4116908000, 59916]
            - mark current block is in-flight state
          80003658 <add_firelist> [4118028000, 59933]
            80002a7c <create_list> [4118798000, 59945]
              - if fire list fire list is empty then create fire list
            80002b30 <create_node> [4286271000, 62474]
            80002c58 <insert> [4342661000, 63326]
              - add current block descriptor into current actor's fire list
          80003244 <task_bind> [4345177000, 63360]
            - bind current actor into block descriptor
          800034e4 <inform_dma> [4345957000, 63373]
            80005a4c <dma_transfer_link> [4347273000, 63392]
             - transmit code
            - traverse current actor's dep_list[
              80005a4c <dma_transfer_link> [4594897000, 67109]
                800021f4 <malloc_LLI> [4595893000, 67122]
                  - malloc a LLI space for current data
                80002b30 <create_node> [4654970000, 68011]
                80002c58 <insert> [4711361000, 68863]
                  - add current token node into token list
                80005878 <lli_setup> [4714912000, 68910]
                  800043f0 <CTL_config> [4717618000, 68946]
                  80005818 <pow> [4724769000, 69062]
                  - assign relative information onto LLI struct
                  ret -> [4730136000, 69130]
            ]
                800059a0 <dma_transfer_channel> [4730913000, 69142]
                  80005544 <DMAC_get_free_channel> [4731216000, 69147]
                  80004580 <CFG_config> [4734595000, 69201]
                  800056b0 <DMAC_CHx_specify_first_lli> [4736317000,69229]
                  8000564c <DMAC_CHx_enable_channel> [4740975000, 69295]
                  - assign current channel's msg descriptor
              ret -> [4746506000, 69355]
        8000348c <ready_free> [4748739000, 69377]
          80002e20 <destroy_node> [4749719000, 69390]
          //////// DMA interrupt awake //////// [4753027000, 69422]
          80002d78 <destroy_list>
          80001f58 <free>
      ]
```

**DMA中断 + 激活Block执行流**
```
80000020 <irq_vec> [4753027000, 69422]
  - store context
  8000176c <irq_handler> [4755063000, 69451]
    8000164c <IRQ_DMA_handler> [4758035000, 69496]
      800052e0 <DMAC_interrupt_handler> [4758338000, 69501]
        80004810 <DMAC_CHx_interrupt_handler> [4765912000, 69618]
          80006334 <dma_transmit_done_handler> [4779130000, 69818]
            - catch transfer information
            8000235c <free_LLI> [4781759000, 69854]
            8000235c <free_LLI> [4816645000, 70381]
            - activate block [4851225000] -> block activated
            80006264 <recycle_garbage> [4851231000, 70908]
              traverse hole transmit token list
                80001f58 <free> [4853217000, 70936]
                  - free data pointer
                80001f58 <free> [4885634000, 71429]
                  - free data descriptor
                80001f58 <free> [4935161000, 72181]
                  - free token descriptor
                80002d78 <destroy_list> [4985321000, 72942]
                  - free hole token list
          - 80006330 ret -> [5108402000, 74803]
  - restore context [5110534000, 74837]
  - retirq [5112293000, 74861]
```

**Block中断执行流**
```
80000020 <irq_vec> [5112473000, 74863]
  - store context
  8000176c <irq_handler> [5114509000, 74892]
    800041cc <block_handler> [5117661000, 74947]
      80003f78 <_set_block_flag> [5118391000, 74958]
      80003fb8 <alloc_result> [5119511000, 74975]
        - parse result information
        80005a4c <dma_transfer_link> [5323296000, 78059]
        80005a4c <dma_transfer_link> [5877447000, 86423]
    8000420c ret -> [6046360000, 88959]
  - restore context [6048822000, 88996]
  - retirq [6050635000, 89021]
```

**DMA中断 + 结果返回执行流**
```
80000020 <irq_vec> [6050775000, 89023]
  - store context
  8000176c <irq_handler> [6052810000, 89052]
    8000164c <IRQ_DMA_handler> [6055783000, 89097]
      800052e0 <DMAC_interrupt_handler> [6056086000, 89102]
        80004810 <DMAC_CHx_interrupt_handler> [6063659000, 89219]
          80006334 <dma_transmit_done_handler> [6076877000, 89419]
            - catch transfer information
            8000235c <free_LLI> [6079506000, 89455]
            8000235c <free_LLI> [6114392000, 89982]
            800061a0 <result_deliver> [6148562000, 90501]
              80002eb8 <read_last> [6149372000, 90513]
                - check if arrival disorder
              800060f0 <linger_insert> [6151074000, 90537]
                80002a7c <create_list> [6152011000, 90551]
                  - create linger list
                ret -> [80002b2c, 93472]
                80001db4 <malloc> [6345417000, 93477]
                  - malloc a linger descriptor space
                80002b30 <create_node> [6409864000, 94452]
                80002c58 <insert> [6475011000, 95438]
                  - add current block into actor's linger list
              80005e4c <scheduler_pass_result> [6477867000, 95478]
                - traverse current actor's all output FIFO[
                  8000397c <put_token> [6481635000, 95534]
                    - put token into FIFO
                  80001db4 <malloc> [6487157000, 95617]
                    - malloc a duplicated token descriptor
                  8000397c <put_token> [6558491000, 96695]
                  80001db4 <malloc> [6563969000, 96777]
                  8000397c <put_token> [6628753000, 97757]
                ]
                80002e20 <destroy_node> [6643264000, 97975]
                  - destroy current fire node
                80002e20 <destroy_node> [6679446000, 98521]
                  - destroy current linger node
                80001f58 <free> [6731429000, 99308]
                  - free current linger descriptor
            80005e24 <_clear_block_flag> [6784478000, 100115]
            800033cc <ready_search> [6785301000, 100129]
              80003274 <ready_create> [6802887000, 100413]
              80003378 <ready_insert> [7147612000, 105621]
              80003274 <ready_create> [7256992000, 107291]
              80003378 <ready_insert> [7661040000, 113391]
          80006460 ret -> [7738992000, 114577]
  - restore context [7740332000, 114598]
  - retirq [7742534000, 114629]
```

