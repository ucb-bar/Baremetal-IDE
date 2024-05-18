#ifndef __LL_RCC_H
#define __LL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


typedef struct {
  __IO uint64_t CLUSTER_RESETS;     // 0x00110000  on reset: 0x00000000_000003FF
  __IO uint64_t CLUSTER_CLKSEL;     // 0x00110008  on reset: 0x00000000_24924924
  __I  uint32_t BOOT;               // 0x00110010  on reset: 0x00000000
  uint32_t RESERVED0[1];
  __IO uint64_t SWITCHER_SEL;       // 0x00110018  on reset: 0x00000000_00000001
  __IO uint64_t HBWIF_RESETS;       // 0x00110020  on reset: 0x00000000_000000FF
  __IO uint64_t UNCLUSTER_CLKSEL;   // 0x00110028  on reset: 0x00000000_00000000
  __IO uint64_t UNCLUSTER_CLKDIV;   // 0x00110030  on reset: 0x00000000_00000004
  __IO uint64_t LBWIF_CLKDIV;       // 0x00110038  on reset: 0x00000000_00000008
} RCC_TypeDef;


#ifdef __cplusplus
}
#endif

#endif /* __LL_RCC_H */