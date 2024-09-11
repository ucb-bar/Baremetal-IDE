#ifndef __UART_H
#define __UART_H

#include "hal.h"
#include "uart_const.h"


void NS16550A_init(void);
Status NS16550A_puts(const uint8_t *data, uint16_t size);


#endif /* __UART_H */