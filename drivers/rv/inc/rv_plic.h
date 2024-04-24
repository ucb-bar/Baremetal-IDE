#ifndef __RV_PLIC_H
#define __RV_PLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t priority_threshold;
  __IO uint32_t claim_complete;
} PLIC_ContextControlEntry_TypeDef;

typedef struct {
  __IO uint32_t priorities[1024];
  __I  uint32_t pendings[1024];
  __IO uint32_t enables[1024];
} PLIC_TypeDef;

// because the maximum struct size is 65535, we need to split PLIC content
typedef struct {
  PLIC_ContextControlEntry_TypeDef context_controls[1024];
} PLIC_ContextControl_TypeDef;


void HAL_PLIC_disable(PLIC_TypeDef *PLICx, PLIC_ContextControl_TypeDef *PLIC_CCx, uint32_t hart_id, uint32_t irq_id);

void HAL_PLIC_enable(PLIC_TypeDef *PLICx, PLIC_ContextControl_TypeDef *PLIC_CCx, uint32_t hart_id, uint32_t irq_id);

void HAL_PLIC_setPriority(PLIC_TypeDef *PLICx, PLIC_ContextControl_TypeDef *PLIC_CCx, uint32_t irq_id, uint32_t priority);

void HAL_PLIC_setPriorityThreshold(PLIC_TypeDef *PLICx, PLIC_ContextControl_TypeDef *PLIC_CCx, uint32_t hart_id, uint32_t priority);

uint32_t HAL_PLIC_claimIRQ(PLIC_TypeDef *PLICx, PLIC_ContextControl_TypeDef *PLIC_CCx, uint32_t hart_id);

void HAL_PLIC_completeIRQ(PLIC_TypeDef *PLICx, PLIC_ContextControl_TypeDef *PLIC_CCx, uint32_t hart_id, uint32_t irq_id);


#ifdef __cplusplus
}
#endif

#endif /* __RV_PLIC_H */