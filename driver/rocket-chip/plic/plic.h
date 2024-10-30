#ifndef __PLIC_H
#define __PLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "metal.h"


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


void plic_disable(PLIC_Type *plic, uint32_t hart_id, uint32_t irq_id);

void plic_enable(PLIC_Type *plic, uint32_t hart_id, uint32_t irq_id);

void plic_set_priority(PLIC_Type *plic, uint32_t irq_id, uint32_t priority);

void plic_cc_set_priority_threshold(PLIC_ContextControl_Type *plic_cc, uint32_t hart_id, uint32_t priority);

uint32_t plic_cc_claim_irq(PLIC_ContextControl_Type *plic_cc, uint32_t hart_id);

void plic_cc_complete_irq(PLIC_ContextControl_Type *plic_cc, uint32_t hart_id, uint32_t irq_id);


#ifdef __cplusplus
}
#endif

#endif /* __PLIC_H */