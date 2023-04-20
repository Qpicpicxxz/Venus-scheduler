# save all General-Purpose(GP) registers to context
# both caller and callee need two pieces of assembly code
# matching the save and restore registers due to the calling behaviour
# Prologue and Epilogue
# abouve will be automatically inserted by the compiler
# to complete the storage and recovery of relevant registers
# struct context *base = &ctx_task;
# base->ra = ra;
.macro reg_save base
	sw ra, 0(\base)
	sw sp, 4(\base)
	sw t0, 16(\base)
	sw t1, 20(\base)
	sw t2, 24(\base)
	sw a0, 36(\base)
	sw a1, 40(\base)
	sw a2, 44(\base)
	sw a3, 48(\base)
	sw a4, 52(\base)
	sw a5, 56(\base)
	sw a6, 60(\base)
	sw a7, 64(\base)
	sw t3, 108(\base)
	sw t4, 112(\base)
	sw t5, 116(\base)
	# we don't save t6 here, due to we have used
	# it as base, we have to save t6 in an extra step
	# outside of reg_save
.endm

# restore all General-Purpose(GP) registers from the context
# struct context *base = &ctx_task;
# ra = base->ra;
# ......
.macro reg_restore base
	lw ra, 0(\base)
	lw sp, 4(\base)
	lw t0, 16(\base)
	lw t1, 20(\base)
	lw t2, 24(\base)
	lw a0, 36(\base)
	lw a1, 40(\base)
	lw a2, 44(\base)
	lw a3, 48(\base)
	lw a4, 52(\base)
	lw a5, 56(\base)
	lw a6, 60(\base)
	lw a7, 64(\base)
	lw t3, 108(\base)
	lw t4, 112(\base)
	lw t5, 116(\base)
	lw t6, 120(\base)
.endm

.text
.globl trap_vector
.align 4
trap_vector:
	# save current control flow's context
	csrrw t6, mscratch, t6
	lui t6, %hi(irq_regs)
	addi t6, t6, %lo(irq_regs)

	reg_save t6

	mv t5, t6
	csrr t6, mscratch
	sw t6, 120(t5)

	# restore the context pointer back to mscratch
	csrw mscratch, t5

	# call C trap handler in trap.c
	csrr a0, mepc
	csrr a1, mcause
	call trap_handler

	# trap_handler return parameter is a0 (may be changed)
	csrw mepc, a0

	# restore context
	csrr t6, mscratch
	reg_restore t6

	# return to wherever we are doing before trap
	# hart would restore previous authority level
	# pc = mepc
	# different privilege has different return instruction
	mret

irq_regs:
	.fill 32,4

.end


