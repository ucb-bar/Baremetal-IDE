/**
 * @file hal_uart.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __HAL_UART_H
#define __HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"
#include "ll_uart.h"

typedef enum {
  UART_MODE_RX          = 0x01,
  UART_MODE_TX          = 0x02,
  UART_MODE_TX_RX       = 0x03,
} UART_Mode;

typedef enum {
  UART_STOPBITS_1       = 0,
  UART_STOPBITS_2       = UART_TXCTRL_NSTOP_MSK,
} UART_StopBits;

typedef struct {
  uint32_t baudrate;  // the default baudrate divisor is 0xAD, 173
  UART_Mode mode;
  UART_StopBits stopbits;
} UART_InitTypeDef;


static inline uint8_t HAL_UART_getRXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK) >> UART_RXCTRL_RXCNT_POS;
}

static inline uint8_t HAL_UART_getTXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK) >> UART_TXCTRL_TXCNT_POS;
}

static inline void HAL_UART_disableRXInterrupt(UART_TypeDef *UARTx) {
  CLEAR_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}

static inline void HAL_UART_enableRXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK);
  SET_BITS(UARTx->RXCTRL, fifo_level << UART_RXCTRL_RXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}

static inline void HAL_UART_disableTXInterrupt(UART_TypeDef *UARTx) {
  CLEAR_BITS(UARTx->IE, UART_IE_TXWM_MSK);
}

static inline void HAL_UART_enableTXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK);
  SET_BITS(UARTx->TXCTRL, fifo_level << UART_TXCTRL_TXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_TXWM_MSK);
}

void HAL_UART_init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_init);

Status HAL_UART_receive(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout);

Status HAL_UART_transmit(UART_TypeDef *UARTx, const uint8_t *data, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_H */