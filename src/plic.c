/* External Interrupt */
/* 
  1. cpu has only one pin connect to the interrupt
  2. PLIC: Platform-Level Interrupt Controller
  - It's a PIC(Programmable Interrupt Controller) designed for RISC-V platform
  3. https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
  Interrupt source: In QEMU it defined IRQ for different interrupt
    enum {
      UART0_IRQ = 10,
      RTC_IRQ = 11,
      VIRTIO_IRQ = 1,
      VIRTIO_COUNT = 8,
      PCIE_IRQ = 0x20, 
      VIRT_PLATFORM_BUS_IRQ = 64, 
    };
    4. PLIC register addressing adopt memory mapping (32bit)
    - Every interrupt points to a register, to config it's priority
    - QEMU-virt support 7th priority
      -0: forbidden
      -1: lowest priority
      -7: highest priority
    5. In QEMU virt, PLIC_BASE = 0x0c0000000L
    0x0c000000 (PLIC_BASE) + offset = Mapped Address of Register
*/
#include "os.h"

void plic_init(void)
{
	int hart = r_tp();
	*(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1;
 
	/*
	 * Enable UART0
	 *
	 * Each global interrupt can be enabled by setting the corresponding 
	 * bit in the enables registers.
	 */
	*(uint32_t*)PLIC_MENABLE(hart)= (1 << UART0_IRQ);

	/* 
	 * Set priority threshold for UART0.
	 *
	 * PLIC will mask all interrupts of a priority less than or equal to threshold.
	 * Maximum threshold is 7.
	 * For example, a threshold value of zero permits all interrupts with
	 * non-zero priority, whereas a value of 7 masks all interrupts.
	 * Notice, the threshold is global for PLIC, not for each interrupt source.
	 */
	*(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;

	/* enable machine-mode external interrupts. */
	w_mie(r_mie() | MIE_MEIE);

	/* enable machine-mode global interrupts. */
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

/* 
 * DESCRIPTION:
 *	Query the PLIC what interrupt we should serve.
 *	Perform an interrupt claim by reading the claim register, which
 *	returns the ID of the highest-priority pending interrupt or zero if there 
 *	is no pending interrupt. 
 *	A successful claim also atomically clears the corresponding pending bit
 *	on the interrupt source.
 * RETURN VALUE:
 *	the ID of the highest-priority pending interrupt or zero if there 
 *	is no pending interrupt.
 */
int plic_claim(void)
{
	int hart = r_tp();
	int irq = *(uint32_t*)PLIC_MCLAIM(hart);
	return irq;
}

/* 
 * DESCRIPTION:
  *	Writing the interrupt ID it received from the claim (irq) to the 
 *	complete register would signal the PLIC we've served this IRQ. 
 *	The PLIC does not check whether the completion ID is the same as the 
 *	last claim ID for that target. If the completion ID does not match an 
 *	interrupt source that is currently enabled for the target, the completion
 *	is silently ignored.
 * RETURN VALUE: none
 */
void plic_complete(int irq)
{
	int hart = r_tp();
	*(uint32_t*)PLIC_MCOMPLETE(hart) = irq;
}

