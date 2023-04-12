include ./common.mk
	
.PHONY : irq
irq: 
	@${CC} ${CFLAGS} -c -o ./src/irq_vector.o ./src/irq_vector.S
	@${CC} ${CFLAGS} -c -o ./src/start.o ./src/start.S
	@${CC} ${CFLAGS} -c -o ./src/hal.o ./src/hal.S
	@${CC} ${CFLAGS} -c -o ./src/irq_kernel.o ./src/irq_kernel.c
	@${CC} ${CFLAGS} -c -o ./src/irq.o ./src/irq.c
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments -o os.elf \
					./src/irq_vector.o \
					./src/start.o \
					./src/hal.o \
					./src/irq_kernel.o \
					./src/irq.o -lc
	@${OBJCOPY} -O binary os.elf os.bin
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@hexdump os.bin > bin.txt

	
.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out  *.map
	@cd src && rm -rf *.o *.bin *.elf *.txt



