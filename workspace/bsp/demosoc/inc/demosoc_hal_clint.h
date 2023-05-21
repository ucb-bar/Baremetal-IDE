/**
 * @file hal_clint.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __DEMOSOC_HAL_CLINT_H
#define __DEMOSOC_HAL_CLINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "demosoc_hal.h"

void HAL_CLINT_triggerSoftwareInterrupt(uint32_t hartid);

uint64_t HAL_CLINT_getTime();

void HAL_CLINT_setTimerInterrupt(uint64_t time);

#ifdef __cplusplus
}
#endif

#endif /* __DEMOSOC_HAL_CLINT_H */