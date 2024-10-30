#ifndef __CLINT_H
#define __CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "metal.h"

/* Peripheral Struct Definition */
typedef struct {
  __IO uint32_t MSIP[4096];                     /** MSIP Registers (1 bit wide) */
  __IO uint64_t MTIMECMP[4095];                 /** MTIMECMP Registers */
  __IO uint64_t MTIME;                          /** Timer Register */
} CLINT_Type;


static inline void clint_clear_software_interrupt(CLINT_Type *clint, uint32_t hartid) {
  CLEAR_BITS(clint->MSIP[hartid], 1U);
}

static inline void clint_trigger_software_interrupt(CLINT_Type *clint, uint32_t hartid) {
  SET_BITS(clint->MSIP[hartid], 1U);
}

/**
 * @brief Get current time
 * 
 * This function returns the current time as stated in the mtime register.
 * 
 * @return uint64_t current time
 */
uint64_t clint_get_time(CLINT_Type *clint);

/**
 * @brief Set target interrupt time
 * 
 * This function sets the target interrupt time for the given hart.
 * 
 * @param hartid hart ID
 * @param time time to set
 */
void clint_set_timer_interrupt_target(CLINT_Type *clint, uint32_t hartid, uint64_t time);


#ifdef __cplusplus
}
#endif

#endif /* __CLINT_H */
