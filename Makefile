include ./common.mk

.PHONY : uart
uart: 
	@${CC} ${CFLAGS} -c -o ./src/start.o ./src/start.S
	@${CC} ${CFLAGS} -c -o ./src/uart.o ./src/uart.c
	@${CC} ${CFLAGS} -c -o ./src/kernel.o ./src/kernel.c
	@${CC} ${CFLAGS} -c -o ./src/printf.o ./src/printf.c
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments -o os.elf \
				./src/start.o \
				./src/uart.o \
				./src/kernel.o \
				./src/printf.o -lc
	@${OBJCOPY} -O binary os.elf os.bin
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@hexdump os.bin > bin.txt

.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out *.zip *.map
	@cd src && rm -rf *.o *.bin *.elf *.txt



