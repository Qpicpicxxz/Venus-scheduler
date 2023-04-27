SOURCE = src
DMA = src/dma
TASK = task
TASKCODE = task/code
TASKBIN = task/cbin

SRCS_ASM = \
		$(SOURCE)/irq_vector.S \
		$(SOURCE)/start.S \
		$(SOURCE)/hal.S \
		$(SOURCE)/mem.S

SRCS_C = \
		$(SOURCE)/kernel.c \
		$(SOURCE)/uart.c \
		$(SOURCE)/init.c \
		$(SOURCE)/printf.c \
		$(SOURCE)/ulib.c \
		$(SOURCE)/irq.c \
		$(SOURCE)/mem_alloc.c \
		$(SOURCE)/mem_block.c \
		$(SOURCE)/mem_free.c \
		$(SOURCE)/linked_list.c \
		$(SOURCE)/fire_check.c \
		$(SOURCE)/fifo.c \
		$(SOURCE)/actor_init.c \
		$(SOURCE)/actor_create.c \
		$(SOURCE)/task_callback.c \
		$(SOURCE)/padding.c \
		$(DMA)/dma_config.c \
		$(DMA)/dma_irq.c \
		$(DMA)/dma_lli_create.c \
		$(DMA)/dma_transfer_done.c \
		$(TASK)/dag.c

SRCS_BIN = $(patsubst $(TASKCODE)/%.c, $(TASKBIN)/%_bin.c, $(wildcard $(TASKCODE)/*.c))
		 
OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)
OBJS += $(SRCS_BIN:.c=.o)
