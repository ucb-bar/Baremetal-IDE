#ifndef __HAL_RCC_H
#define __HAL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ll_rcc.h"


static inline void HAL_RCC_disableCluster(uint32_t cluster_id) {
  SET_BITS(RCC->CLUSTER_RESETS, 1U << cluster_id);
}

static inline void HAL_RCC_enableCluster(uint32_t cluster_id) {
  CLEAR_BITS(RCC->CLUSTER_RESETS, 1U << cluster_id);
}

void HAL_RCC_initSystemClock();


#ifdef __cplusplus
}
#endif

#endif /* __HAL_RCC_H */