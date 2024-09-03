#include "uart.h"

#define UART_NS16550A UART_ADDRESS

void NS16550A_init(void) {
    uart_init();
}

Status NS16550A_puts(const uint8_t *data, uint16_t size) {
    uint8_t *ptr = data;
    while (size > 0) {
        uart_putc(*ptr);
        ptr += sizeof(uint8_t);
        size -= 1;
    }
    return OK;
}

