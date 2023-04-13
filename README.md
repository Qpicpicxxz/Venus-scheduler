# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
---
### **UART测试分支**
在`Makefile`中开启宏`SIM_VCS`即使用VCS调试串口输出

2023/4/11 初始版本
1.  `uart_puts`(一定要加\n)
2.  `printf`(仅支持%p, %s, %d, %x)
3.  `assert`(支持打印assert failed的文件以及行号)
   `assert`能正常打印，但是仿真器报了个错，但依旧能执行
```
Error at 500017.659000 ns: Violate rule (1) -   1MHz <= XIN/N <= 50MHz; XIN/N =600.240096MHz
****************************************************
Error at 500017.659000 ns: Violate rule (1) -   1MHz <= XIN/N <= 50MHz; XIN/N =266.524520MHz
```
4.  暂停vcs仿真：`uart_puts("$stop\n")` `printf("..... $stop\n");`
5.  请求调试器：`asm volatile("ebreak")`


