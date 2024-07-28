#ifndef __UART_H
#define __UART_H

#include "rv.h"

#include "uart_const.h"

Status NS16550A_puts(const uint8_t *data, uint16_t size);

#endif /* __UART_H */