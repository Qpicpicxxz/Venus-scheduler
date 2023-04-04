#include "platform.h"
	
	.equ KERNAL_STACK, 1024	# 64KiB stack space
	
	.global	_start

	.text
_start:
	csrr	t0, mhartid
	mv	tp, t0
	# Set all bytes in the BSS section to zero.
	# to make sure every uninitialized variable to be zero
	la	a0, _bss_start
	la	a1, _bss_end
	bgeu	a0, a1, 2f
1:
	sw	zero, (a0)
	addi	a0, a0, 4
	bltu	a0, a1, 1b
2:
	# Setup stacks, the stack grows from bottom to top, so we put the
	# stack pointer to the very end of the stack range.
	# Prologue
	la	sp, _stack_top
	add	sp, sp, t0

	j	start_kernel		# hart 0 jump to c

	.section .bss.stack
	
	.globl _stack_start
_stack_start:

	.space	KERNAL_STACK

	.globl _stack_top
_stack_top:

	.end				# End of file
