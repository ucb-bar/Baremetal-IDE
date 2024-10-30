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

typedef struct {
  __IO uint64_t STATUS : 11;    // 0x00
  __IO uint8_t MODE;            // 0x08
  __IO uint64_t SRC_ADDR;       // 0x10
  __IO uint64_t DEST_ADDR;      // 0x18
  __IO uint64_t SRCSTRIDE;      // 0x20
  __IO uint32_t COUNT;          // 0x28
  uint32_t RESERVED[4];   
  __IO uint64_t OPERAND_REG[8]; // 0x40  
  __IO uint64_t DEST_REG[8];    // 0x80
} DMA_Type;

#ifdef __cplusplus
}
#endif

#endif /* __LL_DMA_H */