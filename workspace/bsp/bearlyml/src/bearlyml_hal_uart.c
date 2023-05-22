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

  if (READ_BITS(UART_init->mode, 0b01)) {
    SET_BITS(UARTx->RXCTRL, UART_RXCTRL_RXEN_MSK);
  }
  
  if (READ_BITS(UART_init->mode, 0b10)) {
    SET_BITS(UARTx->TXCTRL, UART_TXCTRL_TXEN_MSK);
  }

  CLEAR_BITS(UARTx->TXCTRL, UART_TXCTRL_NSTOP_MSK);
  CLEAR_BITS(UARTx->TXCTRL, UART_init->stopbits);
  
  // baudrate setting
  // f_baud = f_sys / (div + 1)
  UARTx->DIV = (SYS_CLK_FREQ / UART_init->baudrate) - 1;
}

uint8_t HAL_UART_getRXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK) >> UART_RXCTRL_RXCNT_POS;
}

uint8_t HAL_UART_getTXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK) >> UART_TXCTRL_TXCNT_POS;
}

void HAL_UART_disableRXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}

void HAL_UART_enableRXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK);
  SET_BITS(UARTx->RXCTRL, fifo_level << UART_RXCTRL_RXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}

void HAL_UART_disableTXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->IE, UART_IE_TXWM_MSK);
}

void HAL_UART_enableTXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK);
  SET_BITS(UARTx->TXCTRL, fifo_level << UART_TXCTRL_TXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_TXWM_MSK);
}

Status HAL_UART_receive(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout) {
  while (size > 0) {
    while (READ_BITS(UARTx->RXDATA, UART_RXDATA_EMPTY_MSK)) {
      // return TIMEOUT;
    }
    *data = UARTx->RXDATA;
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
