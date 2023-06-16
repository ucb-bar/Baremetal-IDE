/**
 * @file hal_core.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __EXAMPLECHIP_HAL_CORE_H
#define __EXAMPLECHIP_HAL_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "examplechip_hal.h"

void HAL_CORE_disableGlobalInterrupt();

void HAL_CORE_enableGlobalInterrupt();

void HAL_CORE_disableIRQ(IRQn_Type IRQn);

void HAL_CORE_enableIRQ(IRQn_Type IRQn);

void HAL_CORE_clearIRQ(IRQn_Type IRQn);


#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLECHIP_HAL_CORE_H */