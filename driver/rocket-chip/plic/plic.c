/**
 * @file hal_plic.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "plic.h"


void plic_disable(uint32_t hart_id, uint32_t irq_id) {
  uint32_t bit_index = irq_id & 0x1F;
  CLEAR_BITS(PLIC->enables[hart_id], 1 << bit_index);
}

void plic_enable(uint32_t hart_id, uint32_t irq_id) {
  uint32_t bit_index = irq_id & 0x1F;
  SET_BITS(PLIC->enables[hart_id], 1 << bit_index);
}

void plic_set_priority(uint32_t irq_id, uint32_t priority) {
  PLIC->priorities[irq_id] = priority;
}

void plic_set_priority_threshold(uint32_t hart_id, uint32_t priority) {
  PLIC_CC->context_controls[hart_id].priority_threshold = priority;
}

uint32_t plic_claim_irq(uint32_t hart_id) {
  return PLIC_CC->context_controls[hart_id].claim_complete;
}

void plic_complete_irq(uint32_t hart_id, uint32_t irq_id) {
  PLIC_CC->context_controls[hart_id].claim_complete = irq_id;
}
