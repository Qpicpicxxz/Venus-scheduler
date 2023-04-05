# build options
include ../common.mk

ELF = $(patsubst %.c,%.elf,$(SRCS_TASKS))
BINARY = $(patsubst %.c,%.bin,$(SRCS_TASKS))
# title
.PHONY: word
word:
	@echo ""
	@echo "============ COMPILING ============="
	@echo ""

elf: ${ELF}

# in this step will output linker script(ld) warning: task1.elf has a LOAD segment with RWX permissions
# tasks are compiled and linked directly inside the image of the OS
%.elf : %.c
	@${CC} ${CFLAGS} -T task.ld -o $@ $< 2>/dev/null
	@echo "  $< ---> $@"

%.bin : %.elf
	@${OBJCOPY} -O binary $< $@
	@echo "  $< ---> $@"
	


# execute each binary file with python script once and generate a c file respectively
all: word ${BINARY}
	@echo ""
	@echo "============ CONVERTING ============"
	@echo ""
	@python read_bin.py ${BINARY}
	@echo ""
	@echo "============ MOVING CODE ==========="
	@echo ""
	@cp *_bin.c ../
	@echo "  all the generated files have been"
	@echo "  copied to the root directory"
	@echo ""
	@echo "========== MODIFYING LINKER ========"
	@python modify_linker.py
	@echo ""
	@echo "  place every task codes' section"
	@echo ""
	@echo "========== MODIFYING mem.S ========="
	@python modify_mem.py
	@echo ""
	@echo "  define every task codes' description"
	@echo ""
	@echo "======= MODIFYING codeaddr.h ======="
	@python modify_header.py
	@echo ""
	@echo "  declare every task codes' description"
	
.PHONY: code
code: all
	@echo ""
	@echo "=========== DISASSEMBLING ========="
	@echo ""
	@${OBJDUMP} --disassemble-all task1.elf > objdump.txt
	@open objdump.txt
	
.PHONY: clean
clean:
	rm -rf *.elf *.bin *_bin.c *.txt *.map
	@python rollback_linker.py
	@python rollback_mem.py
	@python rollback_header.py

