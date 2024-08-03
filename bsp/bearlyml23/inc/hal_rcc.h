/**
 * @file hal_rcc.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __HAL_RCC_H
#define __HAL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"
#include "ll_rcc.h"
#include "ll_pll.h"




void HAL_RCC_InitSystemClock();

#ifdef __cplusplus
}
#endif

#endif /* __HAL_RCC_H */