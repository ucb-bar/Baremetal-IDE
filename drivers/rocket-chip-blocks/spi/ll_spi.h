#ifndef __LL_SPI_H
#define __LL_SPI_H

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

#ifdef __cplusplus
}
#endif

#endif /* __LL_SPI_H */ 