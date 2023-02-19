display/z $t0
display/z $tp
display/z $mhartid
display/z $mscratch
set disassemble-next-line on
b _start
b start_kernel
b os_main
b task_create
b user_task0
b task1
target remote : 1234
c

