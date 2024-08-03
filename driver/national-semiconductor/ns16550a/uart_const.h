/* adapted from https://github.com/safinsingh/ns16550a.git */

#define UART_ADDRESS              0x10000000
#define LINE_STATUS_REGISTER      0x5
#define LINE_CONTROL_REGISTER     0x3
#define FIFO_CONTROL_REGISTER     0x2
#define INTERRUPT_ENABLE_REGISTER 0x1
#define LINE_STATUS_DATA_READY    0x1

#define UART_NS16550A UART_ADDRESS