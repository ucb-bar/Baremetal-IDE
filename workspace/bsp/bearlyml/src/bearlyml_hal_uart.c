/**
 * @file hal_uart.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bearlyml_hal_uart.h"

void HAL_UART_init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_init) {
  CLEAR_BITS(UARTx->RXCTRL, UART_RXCTRL_RXEN_MSK);
  CLEAR_BITS(UARTx->TXCTRL, UART_TXCTRL_TXEN_MSK);

  if (READ_BITS((uint32_t)UART_init->mode, 0b01)) {
    SET_BITS(UARTx->RXCTRL, UART_RXCTRL_RXEN_MSK);
  }
  
  if (READ_BITS((uint32_t)UART_init->mode, 0b10)) {
    SET_BITS(UARTx->TXCTRL, UART_TXCTRL_TXEN_MSK);
  }

  CLEAR_BITS(UARTx->TXCTRL, UART_TXCTRL_NSTOP_MSK);
  CLEAR_BITS(UARTx->TXCTRL, (uint32_t)UART_init->stopbits);
  
  // baudrate setting
  // f_baud = f_sys / (div + 1)
  UARTx->DIV = (SYS_CLK_FREQ / UART_init->baudrate) - 1;
}

Status HAL_UART_receive(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout) {
  uint32_t rx_data_status;
  while (size > 0) {
    rx_data_status = UARTx->RXDATA;
    while (READ_BITS(rx_data_status, UART_RXDATA_EMPTY_MSK)) {
      rx_data_status = UARTx->RXDATA;
      // return TIMEOUT;
    }
    *data = READ_BITS(rx_data_status, UART_RXDATA_DATA_MSK);
    data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}

Status HAL_UART_transmit(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout) {
  while (size > 0) {
    while (READ_BITS(UARTx->TXDATA, UART_TXDATA_FULL_MSK)) {
      // return TIMEOUT;
    }
    UARTx->TXDATA = *data;
    data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}
