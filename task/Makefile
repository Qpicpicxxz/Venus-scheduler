# build options
include ../common.mk
include ./block.mk

SRCS_TASK = $(wildcard ${TASKCODE}/*.c)

LIB_OBJS = $(SRCS_ASM:.S=.o)
LIB_OBJS += $(SRCS_C:.c=.o)
TASK_OBJS = $(SRCS_TASK:.c=.o)
ELF_FILES = $(patsubst ${TASKCODE}/%.c,${BUILD}/%.elf,$(SRCS_TASK))
BIN_FILES = $(patsubst ${TASKCODE}/%.c,${BUILD}/%.bin,$(SRCS_TASK))
OBJDUMP_FILES = $(patsubst ${TASKCODE}/%.c,${BUILD}/%_objdump.txt,$(SRCS_TASK))

compile: ${LIB_OBJS} ${TASK_OBJS}
	$(foreach elf_file,$(ELF_FILES), \
		$(eval obj_file := $(patsubst ${BUILD}/%.elf,${TASKCODE}/%.o,$(elf_file))) \
		$(eval task_name := $(basename $(notdir $(obj_file)))) \
		${CC} ${CFLAGS} -T task.ld -Wl,--no-warn-rwx-segments -o $(elf_file) ${LIB_OBJS} $(obj_file) -lc; \
		${OBJCOPY} -O binary $(elf_file) $(patsubst ${BUILD}/%.elf,${BUILD}/%.bin,$(elf_file)); \
		${OBJDUMP} --disassemble-all $(elf_file) > $(patsubst ${BUILD}/%.elf,${BUILD}/%_objdump.txt,$(elf_file)); \
	)

%.o : %.c 
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

%.o : %.S task-compile-console
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"


task-compile-console:
	@echo "  ============ COMPILE TASKS ==========="
	@echo ""

# execute each binary file with python script once and generate a c file respectively
all: compile
	@echo ""
	@echo "  ============ GENERATE BINS ==========="
	@echo ""
	@python python/read_bin.py ${BIN_FILES}
	@python python/modify_linker.py
	@python python/modify_mem.py
	@python python/modify_header.py

.PHONY: clean
clean:
	@rm -rf dag.o
	@cd lib && rm -rf *.o
	@cd code && rm -rf *.o
	@cd cbin && rm -rf *
	@cd build && rm -rf *
	rm -rf *.elf *.bin *_bin.c *.txt
	@python python/rollback_linker.py
	@python python/rollback_mem.py
	@python python/rollback_header.py


