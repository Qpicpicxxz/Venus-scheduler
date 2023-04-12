# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband

### PicoRV32分支
1. pico测试只支持中断触发，此版本检测到irq=0x1000的中断即认为是block1发生的，irq=0x0100中断认为是DMA发生的
2. 服务器python会报错，按理来说是这个属性python3.2之后就支持了，因此服务器上跑不起来python脚本，此分支里预存了模拟的三个task_bin.c
```
Traceback (most recent call last):
  File "read_bin.py", line 23, in <module>
    new_value = int.from_bytes(sub_array, byteorder='big')
AttributeError: type object 'int' has no attribute 'from_bytes'
```
3. 此版本是在基于PicoRV的VSC仿真上运行的，其中PicoRV32内核配置为
```
parameter PROGADDR_RESET = 32'h8000_0000;
picorv32 #(
      .ENABLE_MUL     (1),
      .ENABLE_DIV     (1),
      .ENABLE_IRQ     (1),
      .MASKED_IRQ     (32'h 0000_0000),                  // 中断永久禁用掩码
      .LATCHED_IRQ    (32'h 0000_0000),                  // 核是否锁存中断掩码
      .PROGADDR_RESET (PROGADDR_RESET),                  // 上电程序入口地址
      .PROGADDR_IRQ   (PROGADDR_RESET + 32'h 0000_0020), // 中断向量入口地址
      .STACKADDR      (32'h ffff_ffff)                   // 满递减堆栈
	) uut (
		.clk         (clk        ),
		.resetn      (resetn     ),
		.trap        (trap       ),
		.mem_valid   (mem_valid  ),
		.mem_instr   (mem_instr  ),
		.mem_ready   (mem_ready  ),
		.mem_addr    (mem_addr   ),
		.mem_wdata   (mem_wdata  ),
		.mem_wstrb   (mem_wstrb  ),
		.mem_rdata   (mem_rdata  ),

		// IRQ interface
		.irq         (irq        ), //input  [31:0] irq            ,
		.eoi         (eoi        )  //output [31:0] eoi            ,
	);
  ```
  链接脚本相关配置为`os.ld`
```
MEMORY
{
	ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

start = 0;

SECTIONS
{
	.text : {
    . = 0x00000000;
		*(.startup)
		KEEP (*(.startup))
		. = 0x00000020;   // 固定中断向量起始地址为PROGADDR_RESET + 0x20
		*(.startirq)
		KEEP (*(.startirq))
		*(.text .text.*)
	} >ram

  ...

}
```
中断向量相关配置为
```
.section .startup
reset_vec:
	j _start

.section .startirq
irq_vec:
	picorv32_setq_insn(q2, x1)
	picorv32_setq_insn(q3, x2)

  ...
```
  4. 触发中断方式
  ```
initial begin
	irq = 32'h0;
  repeat(10000) @(posedge clk);
	$display("[%t]: testbench: issuing an interrupt",$time);
	irq = 32'h10;     		   //拉高BLOCK中断信号
	repeat(20) @(posedge clk);
  irq = 32'h8;            //在BLOCK中断回调的时候产生的新的中断，PicoRV32不予理会且不会记录
	repeat(20) @(posedge clk);
	irq = 32'h0;
  repeat(2000) @(posedge clk);
	$finish;
end
```
5. 编译出vcs仿真需要的`os.bin`
```
make irq
```
6. 编译完成后会生成`objdump.txt`文件，可查看反汇编
7. 中断端口定义
```
#define IRQ_TIMER 0
#define IRQ_BADINSTR 1
#define IRQ_MEMERROR 2
#define IRQ_DMA 3
#define IRQ_BLOCK 4
```
8. 中断开启以及回调绑定硬件抽象层实现
```
set_handler(IRQ_BLOCK, IRQ_blockhandler);
enable_irq(IRQ_BLOCK);
```
```
void set_handler(uint32_t irq, VoidFunc callback) {
  irq_callback[irq] = callback;
}

void enable_irq(uint32_t irq) {
  EN_Interrupts(1 << irq);
}
```
```
	EN_Interrupts:
		not t0, zero
		picorv32_maskirq_insn(t0, t0) # load interrupts from register
		not a0, a0
		and t0, t0, a0
		picorv32_maskirq_insn(zero, t0) # write back new value
		ret

	# '1' means to diable the interrupt
	DIS_Interrupts:
		not t0, zero
		picorv32_maskirq_insn(t0, t0)
		or t0, t0, a0
		picorv32_maskirq_insn(zero, t0)
		ret
```
9.上下文保存和恢复的寄存器(calling convention里需要caller保存的寄存器)

| Register | ABI  | Use by convention                  |
| :------- | :--- | :--------------------------------- |
| x1       | ra   | Return address                     |
| x2       | sp   | Stack pointer                      |
| x5       | t0   | Temporary register0                |
| x6       | t1   | Temporary register1                |
| x7       | t2   | Temporary register2                |
| x10      | a0   | Function arguments / Return values |
| x11      | a1   | Function arguments                 |
| x12      | a2   | Saved registers                    |
| x13      | a3   | Saved registers                    |
| x14      | a4   | Saved registers                    |
| x15      | a5   | Saved registers                    |
| x16      | a6   | Saved registers                    |
| x17      | a7   | Saved registers                    |
| x28      | ft8  | FP saved registers                 |
| x29      | ft9  | FP saved registers                 |
| x30      | ft10 | FP saved registers                 |
| x31      | ft11 | FP saved registers                 |
