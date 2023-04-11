/* https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAXNUM_CPU 8

/*
 * MemoryMap:
 * https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c
 * virt_memmap[] 
 * 0x00001000 -- boot ROM, provided by qemu
 * 0x02000000 -- CLINT
 * 0x0C000000 -- PLIC
 * 0x10000000 -- UART0
 * 0x10001000 -- virtio disk
 * 0x80000000 -- boot ROM jumps here in machine mode, where we load our kernel
 */

/* puts UART registers here in physical memory */
#define UART0 0x10000000L

/* UART0 interrupt source -> defined in virt.h */
#define UART0_IRQ 10

/* 
 * puts PLIC here(only list Machine mode)
 * #define VIRT_PLIC_HART_CONFIG "MS"
 * #define VIRT_PLIC_NUM_SOURCES 127
 * #define VIRT_PLIC_NUM_PRIORITIES 7
 * #define VIRT_PLIC_PRIORITY_BASE 0x04
 * #define VIRT_PLIC_PENDING_BASE 0x1000
 * #define VIRT_PLIC_ENABLE_BASE 0x2000
 * #define VIRT_PLIC_ENABLE_STRIDE 0x80
 * #define VIRT_PLIC_CONTEXT_BASE 0x200000
 * #define VIRT_PLIC_CONTEXT_STRIDE 0x1000
 * #define VIRT_PLIC_SIZE(__num_context) \
       (VIRT_PLIC_CONTEXT_BASE + (__num_context) * VIRT_PLIC_CONTEXT_STRIDE)
*/
#define PLIC_BASE 0x0c000000L
/* Set priority: mmp address -> BASE + (interrupt - id) * 4 */
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
/* Pending: we read this register to ensure the interrupt happended or not */
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
/* Enable1 / Enalbe2: open or close any interrupt */
#define PLIC_MENABLE(hart) (PLIC_BASE + 0x2000 + (hart) * 0x80)
/* Threadhold: if the current priority lowerthan the priority threadhold, it would be dropped */
#define PLIC_MTHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)
/* Claim/Complete: the same register
 * read -> Claim -> catch the highest interrupt ID
 * write -> Complete -> inform PLIC the interrupt process is done
 */
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)
#define PLIC_MCOMPLETE(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)

#endif /* __PLATFORM_H__ */

