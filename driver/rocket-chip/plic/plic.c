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


void PLIC_disable(uint32_t hart_id, uint32_t irq_id) {
  uint32_t bit_index = irq_id & 0x1F;
  CLEAR_BITS(PLIC->enables[hart_id], 1 << bit_index);
}

void PLIC_enable(uint32_t hart_id, uint32_t irq_id) {
  uint32_t bit_index = irq_id & 0x1F;
  SET_BITS(PLIC->enables[hart_id], 1 << bit_index);
}

void PLIC_setPriority(uint32_t irq_id, uint32_t priority) {
  PLIC->priorities[irq_id] = priority;
}

void PLIC_setPriorityThreshold(uint32_t hart_id, uint32_t priority) {
  PLIC_CC->context_controls[hart_id].priority_threshold = priority;
}

uint32_t PLIC_claimIRQ(uint32_t hart_id) {
  return PLIC_CC->context_controls[hart_id].claim_complete;
}

void PLIC_completeIRQ(uint32_t hart_id, uint32_t irq_id) {
  PLIC_CC->context_controls[hart_id].claim_complete = irq_id;
}
