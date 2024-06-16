#ifndef __LL_PLL_H
#define __LL_PLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


typedef struct {
  __IO uint32_t LOOP_PHASEACQENABLE;        // 0x00
  __IO uint32_t LOOP_UPD_TS;                // 0x04
  __IO uint32_t LOOP_ALG_EN;                // 0x08
  __IO uint32_t LOOP_DSM_EN;                // 0x0C
  __IO uint32_t LOOP_DCOCTRLCODEOVERRIDE;   // 0x10
  __IO uint32_t LOOP_DLF_IC;                // 0x14
  __IO uint32_t LOOP_ALG_IC;                // 0x18
  __IO uint32_t LOOP_ALG_GA;                // 0x1C
  __IO uint32_t LOOP_ALG_CF;                // 0x20
  __IO uint32_t LOOP_DLF_KP;                // 0x24
  __IO uint32_t LOOP_DLF_KI;                // 0x28
  __IO uint32_t BPD_PD;                     // 0x2C
  __IO uint32_t DIV_SEL_CLK_DLF;            // 0x30
  __IO uint32_t DIV_SEL_CLK_DIV;            // 0x34
  __IO uint32_t DIV_PD;                     // 0x38
} PLL_TypeDef;


#ifdef __cplusplus
}
#endif

#endif /* __LL_PLL_H */