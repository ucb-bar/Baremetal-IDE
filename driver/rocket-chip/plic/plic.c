/**
 * @file plic.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "plic.h"


void plic_disable(PLIC_Type *plic, uint32_t hart_id, uint32_t irq_id) {
  uint32_t bit_index = irq_id & 0x1F;
  CLEAR_BITS(plic->enables[hart_id], 1 << bit_index);
}

void plic_enable(PLIC_Type *plic, uint32_t hart_id, uint32_t irq_id) {
  uint32_t bit_index = irq_id & 0x1F;
  SET_BITS(plic->enables[hart_id], 1 << bit_index);
}

void plic_set_priority(PLIC_Type *plic, uint32_t irq_id, uint32_t priority) {
  plic->priorities[irq_id] = priority;
}

void plic_cc_set_priority_threshold(PLIC_ContextControl_Type *plic_cc, uint32_t hart_id, uint32_t priority) {
  plic_cc->context_controls[hart_id].priority_threshold = priority;
}

uint32_t plic_cc_claim_irq(PLIC_ContextControl_Type *plic_cc, uint32_t hart_id) {
  return plic_cc->context_controls[hart_id].claim_complete;
}

void plic_cc_complete_irq(PLIC_ContextControl_Type *plic_cc, uint32_t hart_id, uint32_t irq_id) {
  plic_cc->context_controls[hart_id].claim_complete = irq_id;
}
