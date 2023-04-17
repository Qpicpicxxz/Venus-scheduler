SOURCE = src
DMA = src/dma
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
		$(SOURCE)/devctrl.c \
		$(SOURCE)/printf.c \
		$(SOURCE)/ulib.c \
		$(SOURCE)/irq.c \
		$(SOURCE)/mem_alloc.c \
		$(SOURCE)/mem_block.c \
		$(SOURCE)/mem_free.c \
		$(DMA)/dma_config.c \
		$(DMA)/dma_irq.c \
		$(DMA)/dma_lli_create.c

SRCS_BIN = $(patsubst $(TASKCODE)/%.c, $(TASKBIN)/%_bin.c, $(wildcard $(TASKCODE)/*.c))
		 
OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)
OBJS += $(SRCS_BIN:.c=.o)
