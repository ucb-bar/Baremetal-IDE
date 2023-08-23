/**
 * @file hal_plic.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __EXAMPLECHIP_HAL_PLIC_H
#define __EXAMPLECHIP_HAL_PLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "examplechip_hal.h"

void HAL_PLIC_disable(uint32_t hart_id, uint64_t irq_id);

void HAL_PLIC_enable(uint32_t hart_id, uint64_t irq_id);

void HAL_PLIC_setPriority(uint32_t irq_id, uint64_t priority);

void HAL_PLIC_setPriorityThreshold(uint32_t hart_id, uint32_t priority);

uint32_t HAL_PLIC_claimIRQ(uint32_t hart_id);

void HAL_PLIC_completeIRQ(uint32_t hart_id, uint64_t irq_id);


#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLECHIP_HAL_PLIC_H */