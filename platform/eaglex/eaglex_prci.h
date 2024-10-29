#ifndef __HAL_RCC_H
#define __HAL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv.h"
#include "eaglex.h"


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
} PRCI_Type;

#define PRCI                     ((PRCI_Type *)PRCI_BASE)




static inline void PRCI_disableCluster(uint32_t cluster_id) {
  SET_BITS(PRCI->CLUSTER_RESETS, 1U << cluster_id);
}

static inline void PRCI_enableCluster(uint32_t cluster_id) {
  CLEAR_BITS(PRCI->CLUSTER_RESETS, 1U << cluster_id);
}


#ifdef __cplusplus
}
#endif

#endif /* __HAL_RCC_H */