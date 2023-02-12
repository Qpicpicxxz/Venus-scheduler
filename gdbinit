display/z $t0
display/z $tp
display/z $mhartid
display/z $mscratch
set disassemble-next-line on
b _start
b alloc_stack
b start_kernel
b sched_init
b os_main
b task_create
b user_task0
b task_yield
b schedule
b switch_to
target remote : 1234
c

