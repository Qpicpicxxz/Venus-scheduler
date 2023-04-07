# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband

### PicoRV32分支
1. pico测试只支持中断触发，此版本检测到irq=0x1000的中断即认为是block1发生的，未考虑DMA中断
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
	picorv32_maskirq_insn(zero, zero) # 这个操作也可以放到bootloader里面
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
  repeat(300000) @(posedge clk);   // 至少得300,000个时钟周期，留给scheduler初始化软件代码
  $display("[%t]: testbench: issuing an interrupt",$time);
  $stop;
  irq = 32'h16;                    // 拉高Block中断信号 0x1000
  repeat(10) @(posedge clk);
  $stop;
  irq = 32'h0;                     // 清空外部中断
  repeat(100000) @(posedge clk);
  $finish;
end
```
5. 编译出vcs仿真需要的`os.bin`
```
make code
```
6. 编译完成后会生成`objdump.txt`文件，可查看反汇编
