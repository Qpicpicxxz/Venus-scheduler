.section .rodata

# python end jacklight

.global BSS_START
BSS_START: 	.word _bss_start

.global BSS_END
BSS_END: 	.word _bss_end

.global HEAP_START
HEAP_START: 	.word _heap_start

.global HEAP_SIZE
HEAP_SIZE:	.word _heap_size

.global STACK_END
STACK_END:	.word _stack_top

.global STACK_START
STACK_START:	.word _stack_start

