
.section .text
	.equ KERNAL_STACK, 1024	# stack size for kernel start
	.global	_start

_start:
        # initialize VENUS
	li      x5,0x10010800
	li      x6,0x80000001
	sw      x6,0x4(x5) #VENUS_AHB_DEV_RST_OFFSET
	li      x6,0x00000001
	sw      x6,0x8(x5) #VENUS_APB_DEV_RST_OFFSET
	li      x6,0x8000FFFF
	sw      x6,0x10(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER0
	sw      x6,0x14(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER1
	sw      x6,0x18(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER2
	sw      x6,0x1C(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER3
	sw      x6,0x20(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER4
	sw      x6,0x24(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER5
	sw      x6,0x28(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER6
	sw      x6,0x2C(x5) #VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER7

	# Initialize all registers with 0
	addi	x1, zero, 0
	addi	x2, zero, 0
	addi	x3, zero, 0
	addi	x4, zero, 0
	addi	x5, zero, 0
	addi	x6, zero, 0
	addi	x7, zero, 0
	addi	x8, zero, 0
	addi	x9, zero, 0
	addi	x10, zero, 0
	addi	x11, zero, 0
	addi	x12, zero, 0
	addi	x13, zero, 0
	addi	x14, zero, 0
	addi	x15, zero, 0
	addi	x16, zero, 0
	addi	x17, zero, 0
	addi	x18, zero, 0
	addi	x19, zero, 0
	addi	x20, zero, 0
	addi	x21, zero, 0
	addi	x22, zero, 0
	addi	x23, zero, 0
	addi	x24, zero, 0
	addi	x25, zero, 0
	addi	x26, zero, 0
	addi	x27, zero, 0
	addi	x28, zero, 0
	addi	x29, zero, 0
	addi	x30, zero, 0
	addi	x31, zero, 0
	
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

	.end              # End of file

