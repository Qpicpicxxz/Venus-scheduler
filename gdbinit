display/z $t0
display/z $sp
display/z $mhartid
display/z $mscratch
set disassemble-next-line on
b _start
b start_kernel
b os_main
b schedule
b switch_to
target remote : 1234
c

