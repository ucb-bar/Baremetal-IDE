
#ifndef __HAL_SPI_H
#define __HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"
#include "ll_spi.h"

typedef enum {
  SPI_MODE0,
  SPI_MODE1,
  SPI_MODE2,
  SPI_MODE3
} SPI_Mode;

typedef enum {
  SPI_CSMODE_HW_AUTO,
  SPI_CSMODE_HW_HOLD,
  SPI_CSMODE_SOFTWARE,
} SPI_CSMode;

typedef struct {
  SPI_Mode mode;
  SPI_CSMode csmode;
  uint32_t clock;
} SPI_InitTypeDef;


void HAL_SPI_init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_init);

Status HAL_SPI_receive(SPI_TypeDef *SPIx, uint8_t *data, uint16_t size, uint64_t timeout);

Status HAL_SPI_transmit(SPI_TypeDef *SPIx, uint8_t *data, uint16_t size, uint64_t timeout);

Status HAL_SPI_transmitReceive(SPI_TypeDef *SPIx, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint64_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SPI_H */ 