/**
 * @file hal_clint.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "clint.h"

uint64_t CLINT_getTime() {
  #if RISCV_XLEN == 32
  uint32_t time_lo;
  uint32_t time_hi;

  do {
    time_hi = *((__IO uint32_t *)(&CLINT->MTIME) + 1);
    time_lo = *((__IO uint32_t *)(&CLINT->MTIME));
  } while (*((__IO uint32_t *)(&CLINT->MTIME) + 1) != time_hi);

  return (((uint64_t)time_hi) << 32U) | time_lo;
  #else
  return CLINT->MTIME;
  #endif
}

void CLINT_setTimerInterruptTarget(uint32_t hartid, uint64_t time) {
  #if RISCV_XLEN == 32
  *((__IO uint32_t *)(&CLINT->MTIMECMP[hartid] + 1)) = 0xFFFFFFFF;
  *((__IO uint32_t *)(&CLINT->MTIMECMP[hartid])) = (uint32_t)time;
  *((__IO uint32_t *)(&CLINT->MTIMECMP[hartid])) = (uint32_t)(time >> 32);
  #else
  CLINT->MTIMECMP[hartid] = time;
  #endif
}
