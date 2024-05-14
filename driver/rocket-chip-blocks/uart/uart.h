#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv.h"

#define UART_FIFO_DEPTH                         8

#define UART_TXDATA_DATA_POS                    (0U)
#define UART_TXDATA_DATA_MSK                    (0xFFU << UART_TXDATA_DATA_POS)
#define UART_TXDATA_FULL_POS                    (31U)
#define UART_TXDATA_FULL_MSK                    (0x1U << UART_TXDATA_FULL_POS)
#define UART_RXDATA_DATA_POS                    (0U)
#define UART_RXDATA_DATA_MSK                    (0xFFU << UART_RXDATA_DATA_POS)
#define UART_RXDATA_EMPTY_POS                   (31U)
#define UART_RXDATA_EMPTY_MSK                   (0x1U << UART_RXDATA_EMPTY_POS)
#define UART_TXCTRL_TXEN_POS                    (0U)
#define UART_TXCTRL_TXEN_MSK                    (0x1U << UART_TXCTRL_TXEN_POS)
#define UART_TXCTRL_NSTOP_POS                   (1U)
#define UART_TXCTRL_NSTOP_MSK                   (0x1U << UART_TXCTRL_NSTOP_POS)
#define UART_TXCTRL_TXCNT_POS                   (16U)
#define UART_TXCTRL_TXCNT_MSK                   (0x7U << UART_RXCTRL_RXCNT_POS)
#define UART_RXCTRL_RXEN_POS                    (0U)
#define UART_RXCTRL_RXEN_MSK                    (0x1U << UART_RXCTRL_RXEN_POS)
#define UART_RXCTRL_RXCNT_POS                   (16U)
#define UART_RXCTRL_RXCNT_MSK                   (0x7U << UART_RXCTRL_RXCNT_POS)
#define UART_IE_TXWM_POS                        (0U)
#define UART_IE_TXWM_MSK                        (0x1U << UART_IE_TXWM_POS)
#define UART_IE_RXWM_POS                        (1U)
#define UART_IE_RXWM_MSK                        (0x1U << UART_IE_RXWM_POS)
#define UART_IP_TXWM_POS                        (0U)
#define UART_IP_TXWM_MSK                        (0x1U << UART_IP_TXWM_POS)
#define UART_IP_RXWM_POS                        (1U)
#define UART_IP_RXWM_MSK                        (0x1U << UART_IP_RXWM_POS)
#define UART_DIV_DIV_POS                        (0U)
#define UART_DIV_DIV_MSK                        (0xFFU << UART_DIV_DIV_POS)


/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t TXDATA;                         /** Transmit data register */
  __I  uint32_t RXDATA;                         /** Receive data register */
  __IO uint32_t TXCTRL;                         /** Transmit control register */
  __IO uint32_t RXCTRL;                         /** Receive control register */
  __IO uint32_t IE;                             /** UART interrupt enable */
  __I  uint32_t IP;                             /** UART interrupt pending */
  __IO uint32_t DIV;                            /** Baud rate divisor */
} UART_TypeDef;


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


static inline uint8_t UART_getRXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK) >> UART_RXCTRL_RXCNT_POS;
}

static inline uint8_t UART_getTXFIFODepth(UART_TypeDef *UARTx) {
  return READ_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK) >> UART_TXCTRL_TXCNT_POS;
}

static inline void UART_disableRXInterrupt(UART_TypeDef *UARTx) {
  CLEAR_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}

static inline void UART_enableRXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK);
  SET_BITS(UARTx->RXCTRL, fifo_level << UART_RXCTRL_RXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}

static inline void UART_disableTXInterrupt(UART_TypeDef *UARTx) {
  CLEAR_BITS(UARTx->IE, UART_IE_TXWM_MSK);
}

static inline void UART_enableTXInterrupt(UART_TypeDef *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->TXCTRL, UART_TXCTRL_TXCNT_MSK);
  SET_BITS(UARTx->TXCTRL, fifo_level << UART_TXCTRL_TXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_TXWM_MSK);
}

void UART_init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_init);

Status UART_receive(UART_TypeDef *UARTx, uint8_t *data, uint16_t size, uint32_t timeout);

Status UART_transmit(UART_TypeDef *UARTx, const uint8_t *data, uint16_t size, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif /* __UART_H */