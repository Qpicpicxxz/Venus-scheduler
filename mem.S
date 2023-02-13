.section .rodata
.global HEAP_START
HEAP_START: .word _heap_start

.global HEAP_SIZE
HEAP_SIZE: .word _heap_size

.global TEXT_START
TEXT_START: .word _text_start

.global TEXT_END
TEXT_END: .word _text_end

.global DATA_START
DATA_START: .word _data_start

.global DATA_END
DATA_END: .word _data_end

.global RODATA_START
RODATA_START: .word _rodata_start

.global RODATA_END
RODATA_END: .word _rodata_end

.global TASK1_START
TASK1_START: .word _task1_start

.global TASK1_END
TASK1_END:	.word _task1_end

.global TASK2_START
TASK2_START: .word _task2_start

.global TASK2_END
TASK2_END:	.word _task2_end

.global TASK3_START
TASK3_START: .word _task3_start

.global TASK3_END
TASK3_END:	.word _task3_end

#.global [TASKNAME]_START
#[TASKNAME]_START: .word _[taskName]_start

#.global [TASKNAME]_END
#[TASKNAME]_END:	.word _[taskName]_end

.global BSS_START
BSS_START: .word _bss_start

.global BSS_END
BSS_END: .word _bss_end

