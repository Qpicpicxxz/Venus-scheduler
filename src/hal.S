#include "irq_instr.S"

.section .text
	.global EN_Interrupts
	.global DIS_Interrupts
	

	# maskirq: 0 -> enable 1 -> disable
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
