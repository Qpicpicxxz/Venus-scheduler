#ifndef __HW_UART_H_
#define __HW_UART_H_

/* [P.117] DW_apb_uart Memory Map */
#define UART_RBR_OFFSET       0x00  // Receive Buffer Register - 存放接收到的数据字节，可通过该寄存器读取接收到的数据
#define UART_THR_OFFSET       0x00  // Transmitter Holding Register - 存放要发送的数据字节，可通过该寄存器写入数据进行发送
#define UART_DLL_OFFSET       0x00  // Divisor Latch Low Register - 低位分频器寄存器
#define UART_IER_OFFSET       0x04  // Divisor Latch High Register - 与DLL一起用于设置串口的通信波特率，可通过这两个寄存器设置波特率的分频系数
#define UART_DLH_OFFSET       0x04  // Interrupt Enable Register - 用于控制串口中断的产生和屏蔽
#define UART_IIR_OFFSET       0x08  // Interrupt Identification Register - 用于识别串口中断的类型和原因
#define UART_FCR_OFFSET       0x08  // FIFO Control Register - 用于控制串口的FIFO缓冲区(使能/禁用FIFO、清空FIFO、设置FIFO深度)
#define UART_LCR_OFFSET       0x0c  // Line Control Register - 用于设置数据位数、校验位、停止串口位等通信参数
#define UART_MCR_OFFSET       0x10  // Mode Control Register - 用于控制串口通信的一些额外功能，例如RTS、DTR、Loopback等
#define UART_LSR_OFFSET       0x14  // Line Status Register - 用于反应串口通信状态，比如数据是否准备好、是否接受数据以及是否完成等
#define UART_MSR_OFFSET       0x18  // Mode Status Register - 用于反应串口通信的一些额外状态，例如CTS、DSR、RI等
#define UART_SCR_OFFSET       0x1c  // Scratchpad Register - 用于用户自定义数据的透传，通常不用
#define UART_LPDLL_OFFSET     0x20
#define UART_LPDLH_OFFSET     0x24  // Low-power Divisor Latch Low/High Register - 用于在低功耗模式下设置串口通信的波特率分频系数
#define UART_SRBR_LOW_OFFSET  0x30  // Serial Receive Buffer Register (Low)
#define UART_SRBR_HIGH_OFFSET 0x6c  // Serial Receive Buffer Register (High)
#define UART_STHR_LOW_OFFSET  0x30  // Serial Transmit Holding Register (Low)
#define UART_STHR_HIGH_OFFSET 0x6c  // Serial Transmit Holding Register (High)
#define UART_FAR_OFFSET       0x70  // FIFO Access Register
#define UART_TFR_OFFSET       0x74  // Transmit FIFO Reset
#define UART_RFW_OFFSET       0x78  // Receiver FIFO Reset
#define UART_USR_OFFSET       0x7c  // UART Status Register
#define UART_TFL_OFFSET       0x80  // Transmit FIFO Level
#define UART_RFL_OFFSET       0x84  // Receive FIFO Level
#define UART_SRR_OFFSET       0x88  // Software Reset Register
#define UART_SRTS_OFFSET      0x8c  // Shadow Request to Send
#define UART_SBCR_OFFSET      0x90  // Shadow Break Control Register
#define UART_SDMAM_OFFSET     0x94  // Shadow DMA Mode
#define UART_SFE_OFFSET       0x98  // Shadow FIFO Enable
#define UART_SRT_OFFSET       0x9c  // Shadow RCVR Trigger
#define UART_STET_OFFSET      0xa0  // Shadow TX Empty Trigger
#define UART_HTX_OFFSET       0xa4  // Halt Transmission
#define UART_DMASA_OFFSET     0xa8  // DMA Software Acknowledge

#define UART_FIFO_DEPTH 60

#endif /* __HW_UART_H_ */
