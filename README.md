# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
**Tips**
1.  暂停vcs仿真：`printf("anything u want to console $stop\n")`
2. scheduler程序起始地址：`.PROGADDR_RESET (32'h 8000_0000)`
3. irq vector地址：`.PROGADDR_IRQ (32'h 8000_0020)`
4. custom irq is level triggered：`.LATCHED_IRQ (32'h 0000_0007)`
5. 最好都用`WRITE_BUTST_32`
6. pico不支持中断嵌套，需要等待中断发生的时候要记得返回上一个中断，或者`picorv32_waitirq_insn(irq)`(在汇编里才能用)
7. 启动某些block的uart输出功能:`../src/rtl/venus_soc_pkg.sv`里的`VENUS_BLOCK_DEBUG_UART_AVAILABLE_LIST`以及`VENUS_BLOCK_UART_PRINT_TO_VCS_CONSOLE_AND_FILE_AVAILABLE_LIST`配置
