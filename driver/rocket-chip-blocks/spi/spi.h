#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"

#define SPI_CSMODE_AUTO                   0x00U
#define SPI_CSMODE_HOLD                   0x02U
#define SPI_CSMODE_OFF                    0x03U

#define SPI_SCKDIV_DIV_POS                      (0U)
#define SPI_SCKDIV_DIV_MSK                      (0x7FFU << SPI_SCKDIV_DIV_POS)
#define SPI_SCKMODE_PHA_POS                     (0U)
#define SPI_SCKMODE_PHA_MSK                     (0x1U << SPI_SCKMODE_PHA_POS)
#define SPI_SCKMODE_POL_POS                     (1U)
#define SPI_SCKMODE_POL_MSK                     (0x1U << SPI_SCKMODE_POL_POS)
#define SPI_CSID_CSID_POS                       (0U)
#define SPI_CSID_CSID_MSK                       (0xFFFFFFFFU << SPI_CSID_CSID_POS)
#define SPI_CSDEF_CSDEF_POS                     (0U)
#define SPI_CSDEF_CSDEF_MSK                     (0xFFFFFFFFU << SPI_CSDEF_CSDEF_POS)
#define SPI_CSMODE_MODE_POS                     (0U)
#define SPI_CSMODE_MODE_MSK                     (0x3U << SPI_CSMODE_MODE_POS)
#define SPI_TXDATA_DATA_POS                     (0U)
#define SPI_TXDATA_DATA_MSK                     (0xFFU << SPI_TXDATA_DATA_POS)
#define SPI_TXDATA_FULL_POS                     (31U)
#define SPI_TXDATA_FULL_MSK                     (0x1U << SPI_TXDATA_FULL_POS)
#define SPI_RXDATA_DATA_POS                     (0U)
#define SPI_RXDATA_DATA_MSK                     (0xFFU << SPI_RXDATA_DATA_POS)
#define SPI_RXDATA_EMPTY_POS                    (31U)
#define SPI_RXDATA_EMPTY_MSK                    (0x1U << SPI_RXDATA_EMPTY_POS)


/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t SCKDIV;
  __IO uint32_t SCKMODE;
  uint32_t RESERVED0[2];
  __IO uint32_t CSID;
  __IO uint32_t CSDEF;
  __IO uint32_t CSMODE;
  uint32_t RESERVED1[3];
  __IO uint32_t DELAY0;
  __IO uint32_t DELAY1;
  uint32_t RESERVED2[4];
  __IO uint32_t FMT;
  uint32_t RESERVED3[1];
  __IO uint32_t TXDATA;
  __IO uint32_t RXDATA;
  __IO uint32_t TXMARK;
  __IO uint32_t RXMARK;
  uint32_t RESERVED4[6];
  __IO uint32_t IE;
  __IO uint32_t IP;  
} QSPI_TypeDef;

typedef struct {
  __IO uint32_t SCKDIV;
  __IO uint32_t SCKMODE;
  uint32_t RESERVED0[2];
  __IO uint32_t CSID;
  __IO uint32_t CSDEF;
  __IO uint32_t CSMODE;
  uint32_t RESERVED1[3];
  __IO uint32_t DELAY0;
  __IO uint32_t DELAY1;
  uint32_t RESERVED2[4];
  __IO uint32_t FMT;
  uint32_t RESERVED3[1];
  __IO uint32_t TXDATA;
  __IO uint32_t RXDATA;
  __IO uint32_t TXMARK;
  __IO uint32_t RXMARK;
  uint32_t RESERVED4[6];
  __IO uint32_t IE;
  __IO uint32_t IP;  
} SPI_TypeDef;



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

#ifndef QSPI0
  #define QSPI0_BASE                0x10030000U
  #define QSPI0                     ((QSPI0_TypeDef *)QSPI0_BASE)
#endif

void SPI_init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_init);

Status SPI_receive(SPI_TypeDef *SPIx, uint8_t *data, uint16_t size, uint64_t timeout);

Status SPI_transmit(SPI_TypeDef *SPIx, uint8_t *data, uint16_t size, uint64_t timeout);

Status SPI_transmitReceive(SPI_TypeDef *SPIx, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint64_t timeout);



#ifdef __cplusplus
}
#endif

#endif /* __SPI_H */ 