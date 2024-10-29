#ifndef __LL_PLIC_H
#define __LL_PLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"


/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t priority_threshold;
  __IO uint32_t claim_complete;
} PLIC_ContextControlEntry_Type;

typedef struct {
  __IO uint32_t priorities[1024];
  __I  uint32_t pendings[1024];
  __IO uint32_t enables[1024];
} PLIC_Type;

// because the maximum struct size is 65535, we need to split PLIC content
typedef struct {
  PLIC_ContextControlEntry_Type context_controls[1024];
} PLIC_ContextControl_Type;


#ifndef PLIC_BASE
  #define PLIC_BASE                 0x0C000000U
  #define PLIC                      ((PLIC_Type *)PLIC_BASE)
  #define PLIC_CC                   ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))
#endif


void plic_disable(uint32_t hart_id, uint32_t irq_id);

void plic_enable(uint32_t hart_id, uint32_t irq_id);

void plic_set_priority(uint32_t irq_id, uint32_t priority);

void plic_set_priority_threshold(uint32_t hart_id, uint32_t priority);

uint32_t plic_claim_irq(uint32_t hart_id);

void plic_complete_irq(uint32_t hart_id, uint32_t irq_id);


#ifdef __cplusplus
}
#endif

#endif /* __LL_PLIC_H */