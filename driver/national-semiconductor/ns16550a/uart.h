#ifndef __UART_H
#define __UART_H

#include "metal.h"
#include "uart_const.h"


void ns16550a_init(void);

Status ns16550a_puts(const uint8_t *data, uint16_t size);


#endif /* __UART_H */