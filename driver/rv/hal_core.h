/**
 * @file hal_core.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __HAL_CORE_H
#define __HAL_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "rv.h"
#include "rv_arch.h"
#include "core.h"

typedef enum {
  UserSoftwareInterrupt         = 0,
  SupervisorSoftwareInterrupt   = 1,
  HypervisorSoftwareInterrupt   = 2,
  MachineSoftwareInterrupt      = 3,
  UserTimerInterrupt            = 4,
  SupervisorTimerInterrupt      = 5,
  HypervisorTimerInterrupt      = 6,
  MachineTimerInterrupt         = 7,
  UserExternalInterrupt         = 8,
  SupervisorExternalInterrupt   = 9,
  HypervisorExternalInterrupt   = 10,
  MachineExternalInterrupt      = 11,
} InterruptType;


static inline size_t getHartId() {
  return READ_CSR("mhartid");
}

static inline uint64_t getCycles() {
  return READ_CSR("mcycle");
}

static inline void disableGlobalInterrupt() {
  CLEAR_CSR_BITS("mstatus", 1U << 3U);
}

static inline void enableGlobalInterrupt() {
  SET_CSR_BITS("mstatus", 1U << 3U);
}

static inline void disableIRQ(InterruptType IRQn) {
  CLEAR_CSR_BITS("mie", 1U << (uint32_t)IRQn);
}

static inline void enableIRQ(InterruptType IRQn) {
  SET_CSR_BITS("mie", 1U << (uint32_t)IRQn);
}

static inline void clearIRQ(InterruptType IRQn) {
  CLEAR_CSR_BITS("mip", 1U << (uint32_t)IRQn);
}

void delay(uint64_t time);


#ifdef __cplusplus
}
#endif

#endif /* __HAL_CORE_H */