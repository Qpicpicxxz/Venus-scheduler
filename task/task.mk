# build options
include ./common.mk

# task.elf and task.bin
ELF = $(patsubst %.c,%.elf,$(SRCS_TASKS))
BINARY = $(patsubst %.c,%.bin,$(SRCS_TASKS))

# label
.PHONY: word
word:
	@echo ""
	@echo "======== COMPILING ========"
	@echo ""

elf: ${ELF}

task%.elf : task%.c
	@${CC} ${CFLAGS} -o $@ $<
	@echo "  $< ---> $@"

task%.bin : task%.elf
	@${OBJCOPY} -O binary $< $@
	@echo "  $< ---> $@"

# execute each binary file with python script once and generate a c file respectively
task: word ${BINARY}
	@echo ""
	@echo "======== CONVERTING ========"
	@echo ""
	@python readbin.py ${BINARY}
	@echo ""
	@echo "======== MOVING CODE ========"
	@echo ""
	@cp *_bin.c ../
	@echo "  all the generated files have been copied to the root directory"
	@echo ""

.PHONY: taskclean
taskclean:
	rm -rf *.elf *.bin *_bin.c

