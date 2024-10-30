/**
 * @file hal_rcc.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __LL_DMA_H
#define __LL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"

#define DMA_ERR_POS                        (0U)
#define DMA_ERR_MSK                        (0x1U << DMA_ERR_POS)
#define DMA_COMPL_POS                      (1U)
#define DMA_COMPL_MSK                      (0x1U << DMA_COMPL_POS)
#define DMA_INPROG_POS                     (2U)
#define DMA_INPROG_MSK                     (0x1U << DMA_INPROG_POS)
#define DMA_BADMODE_POS                    (3U)
#define DMA_BADMODE_MSK                    (0x1U << DMA_BADMODE_POS)
#define DMA_SRCALIGN_POS                   (4U)
#define DMA_SRCALIGN_MSK                   (0x1U << DMA_SRCALIGN_POS)
#define DMA_DSTALIGN_POS                   (5U)
#define DMA_DSTALIGN_MSK                   (0x1U << DMA_DSTALIGN_POS)
#define DMA_STRALIGN_POS                   (6U)
#define DMA_STRALIGN_MSK                   (0x1U << DMA_STRALIGN_POS)
#define DMA_CNTERR_POS                     (7U)
#define DMA_CNTERR_MSK                     (0x1U << DMA_CNTERR_POS)
#define DMA_DENYR_POS                      (8U)
#define DMA_DENYR_MSK                      (0x1U << DMA_DENYR_POS)
#define DMA_CORRUPTR_POS                   (9U)
#define DMA_CORRUPTR_MSK                   (0x1U << DMA_CORRUPTR_POS)
#define DMA_DENYW_POS                      (10U)
#define DMA_DENYW_MSK                      (0x1U << DMA_DENYW_POS)

typedef struct {
  __I uint64_t STATUS;    // 0x00
  __IO uint8_t MODE;            // 0x08
  uint8_t RESERVED0;
  __IO uint64_t SRC_ADDR;       // 0x10
  __IO uint64_t DEST_ADDR;      // 0x18
  __IO uint64_t SRCSTRIDE;      // 0x20
  __IO uint32_t COUNT;          // 0x28
  uint32_t RESERVED[5];   
  __IO uint64_t OPERAND_REG[8]; // 0x40  
  __I int16_t DEST_REG[32];    // 0x80
} DMA_Type;

#ifdef __cplusplus
}
#endif

#endif /* __LL_DMA_H */