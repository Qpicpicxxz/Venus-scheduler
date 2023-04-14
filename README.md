# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
**Tips**
1.  暂停vcs仿真：`printf("anything u want to console $stop\n")`
2. scheduler程序起始地址：`.PROGADDR_RESET (32'h 8000_0000)`
3. irq vector地址：`.PROGADDR_IRQ (32'h 8000_0020)`
4. custom irq is level triggered：`.LATCHED_IRQ (32'h 0000_0007)`
5. 最好都用`WRITE_BUTST_32`
