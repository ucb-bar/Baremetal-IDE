/**
 * @file hal_gpio.h
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __BEARLYML_HAL_GPIO_H
#define __BEARLYML_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bearlyml_hal.h"

typedef enum {
  GPIO_PIN_0 = 0b0001U,
  GPIO_PIN_1 = 0b0010U,
  GPIO_PIN_2 = 0b0100U,
  GPIO_PIN_3 = 0b1000U,
} GPIO_PIN;

typedef enum {
  GPIO_MODE_INPUT,
  GPIO_MODE_OUTPUT,
  GPIO_MODE_INOUT,
  GPIO_MODE_ALTERNATE_FUNCTION
} GPIO_Mode;

typedef enum {
  GPIO_PULL_NONE,
  GPIO_PULL_UP
} GPIO_Pull;

typedef enum {
  GPIO_DS_WEAK,
  GPIO_DS_STRONG
} GPIO_DriveStrength;

typedef struct {
  GPIO_Mode mode;
  GPIO_Pull pull;
  GPIO_DriveStrength drive_strength;
} GPIO_InitTypeDef;

void HAL_GPIO_init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_init, GPIO_PIN pin);

uint8_t HAL_GPIO_readPin(GPIO_TypeDef *GPIOx, GPIO_PIN pin);

void HAL_GPIO_writePin(GPIO_TypeDef *GPIOx, GPIO_PIN pin, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* __BEARLYML_HAL_GPIO_H */