# build options
include ../common.mk

ELF = $(patsubst %.c,%.elf,$(SRCS_TASKS))
BINARY = $(patsubst %.c,%.bin,$(SRCS_TASKS))
# title
.PHONY: word
word:
	@echo ""
	@echo "\033[0;1m"=========== COMPILING TASK CODE ==========="\033[0m"
	@echo ""

elf: ${ELF}

# in this step will output linker script(ld) warning: task1.elf has a LOAD segment with RWX permissions
# tasks are compiled and linked directly inside the image of the OS
%.elf : %.c
	@${CC} ${CFLAGS} -T task.ld -o $@ $< 2>/dev/null
	@echo "  task/$< ---> $@"

%.bin : %.elf
	@${OBJCOPY} -O binary $< $@
	@echo "  task/$< ---> $@"
	


# execute each binary file with python script once and generate a c file respectively
all: word ${BINARY}
	@echo ""
	@echo "\033[0;1m"=========== CONVERTING BIN TO C ==========="\033[0m"
	@echo ""
	@python read_bin.py ${BINARY}
	@echo ""
	@echo "\033[0;1m"============ MODIFYING os.ld =============="\033[0m"
	@python modify_linker.py
	@echo ""
	@echo "  define every task codes' section"
	@echo "  in scheduler's linker script"
	@echo ""
	@echo "\033[0;1m"============ MODIFYING mem.S =============="\033[0m"
	@python modify_mem.py
	@echo ""
	@echo "  capture every task codes' information"
	@echo "  in scheduler's memory mapping file"
	@echo ""
	@echo "\033[0;1m"========== MODIFYING codeaddr.h ==========="\033[0m"
	@python modify_header.py
	@echo ""
	@echo "  declare every task codes' information"
	# @echo ""
	# @echo "========== CREATING dag.c =========="
	# @python generate_dag.py
	# @echo ""
	# @echo "  generate a DAG description randomly"
	
.PHONY: code
code: all
	@echo ""
	@echo "\033[0;1m"============== DISASSEMBLING ============="\033[0m"
	@echo ""
	@${OBJDUMP} --disassemble-all task1.elf > objdump.txt
	@open objdump.txt
	
.PHONY: clean
clean:
	rm -rf *.elf *.bin *_bin.c *.txt *.map *.o
	@python rollback_linker.py
	@python rollback_mem.py
	@python rollback_header.py

