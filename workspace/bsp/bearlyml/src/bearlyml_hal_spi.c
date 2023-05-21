
#include "bearlyml_hal_spi.h"

void HAL_SPI_init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_init) {
  SPIx->SCKDIV = SYS_CLK_FREQ / (2 * SPI_init->clock) - 1;
  
  switch (SPI_init->mode) {
    case SPI_MODE0:
      // inactive low; data sampled on rising edge and shifted out on the falling edge
      CLEAR_BITS(SPIx->SCKMODE, SPI_SCKMODE_PHA_MSK);
      CLEAR_BITS(SPIx->SCKMODE, SPI_SCKMODE_POL_MSK);
      break;
    case SPI_MODE1:
      // inactive low; data sampled on falling edge and shifted out on the rising edge
      SET_BITS(SPIx->SCKMODE, SPI_SCKMODE_PHA_MSK);
      CLEAR_BITS(SPIx->SCKMODE, SPI_SCKMODE_POL_MSK);
      break;
    case SPI_MODE2:
      // inactive high; data sampled on falling edge and shifted out on the rising edge
      CLEAR_BITS(SPIx->SCKMODE, SPI_SCKMODE_PHA_MSK);
      SET_BITS(SPIx->SCKMODE, SPI_SCKMODE_POL_MSK);
      break;
    case SPI_MODE3:
      // inactive high; data sampled on falling edge and shifted out on the rising edge
      SET_BITS(SPIx->SCKMODE, SPI_SCKMODE_PHA_MSK);
      SET_BITS(SPIx->SCKMODE, SPI_SCKMODE_POL_MSK);
      break;
  }

  switch (SPI_init->csmode) {
    case SPI_CSMODE_HW_AUTO:
      SPIx->CSMODE = SPI_CSMODE_AUTO;
      break;
    case SPI_CSMODE_HW_HOLD:
      SPIx->CSMODE = SPI_CSMODE_HOLD;
      break;
    case SPI_CSMODE_SOFTWARE:
      SPIx->CSMODE = SPI_CSMODE_OFF;
      break;
  }
}

Status HAL_SPI_receive(SPI_TypeDef *SPIx, uint8_t *data, uint16_t size, uint64_t timeout) {
  while (size > 0) {
    while (READ_BITS(SPIx->RXDATA, SPI_RXDATA_EMPTY_MSK)) {
      // return TIMEOUT;
    }
    *data = SPIx->RXDATA;
    data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}

Status HAL_SPI_transmit(SPI_TypeDef *SPIx, uint8_t *data, uint16_t size, uint64_t timeout) {
  while (size > 0) {
    while (READ_BITS(SPIx->TXDATA, SPI_TXDATA_FULL_MSK)) {
      // return TIMEOUT;
    }
    SPIx->TXDATA = *data;
    data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}

Status HAL_SPI_transmitReceive(SPI_TypeDef *SPIx, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint64_t timeout) {
  while (size > 0) {
    while (READ_BITS(SPIx->TXDATA, SPI_TXDATA_FULL_MSK)) {
      // return TIMEOUT;
    }
    SPIx->TXDATA = *tx_data;
    tx_data += sizeof(uint8_t);
    while (READ_BITS(SPIx->RXDATA, SPI_RXDATA_EMPTY_MSK)) {
      // return TIMEOUT;
    }
    *rx_data = SPIx->RXDATA;
    rx_data += sizeof(uint8_t);
    size -= 1;
  }
  return OK;
}
