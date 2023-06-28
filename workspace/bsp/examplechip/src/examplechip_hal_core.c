/**
 * @file hal_core.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "examplechip_hal_core.h"

void HAL_CORE_disableGlobalInterrupt() {
  CLEAR_CSR_BITS("mstatus", 1U << 3U);
}

void HAL_CORE_enableGlobalInterrupt() {
  SET_CSR_BITS("mstatus", 1U << 3U);
}

void HAL_CORE_disableIRQ(IRQn_Type IRQn) {
  CLEAR_CSR_BITS("mie", 1U << (uint32_t)IRQn);
}

void HAL_CORE_enableIRQ(IRQn_Type IRQn) {
  SET_CSR_BITS("mie", 1U << (uint32_t)IRQn);
}
