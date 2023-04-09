#include "irq_instr.S"

.section .text
	.global EN_Interrupts
	.global DIS_Interrupts
	

	# maskirq: 0 -> enable 1 -> disable
	# 0010
	EN_Interrupts:
		not t0, zero   # 1111
		picorv32_maskirq_insn(t0, t0) # load interrupts from register
		not a0, a0     # 1101
		and t0, t0, a0 # 1101
		picorv32_maskirq_insn(zero, t0) # write back new value
		mv a0, t0
		ret
	
	# '1' means to diable the interrupt
	DIS_Interrupts:
		not t0, zero
		picorv32_maskirq_insn(t0, t0)
		or t0, t0, a0
		picorv32_maskirq_insn(zero, t0)
		mv a0, t0
		ret
