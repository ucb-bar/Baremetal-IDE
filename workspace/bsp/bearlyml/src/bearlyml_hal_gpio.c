/**
 * @file hal_gpio.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "bearlyml_hal_gpio.h"

void HAL_GPIO_init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_init, GPIO_PIN pin) {
  if (GPIO_init->mode == GPIO_MODE_INPUT || GPIO_init->mode == GPIO_MODE_OUTPUT) {
    SET_BITS(GPIOx->INPUT_EN, (uint32_t)pin);
  }
  if (GPIO_init->mode == GPIO_MODE_OUTPUT || GPIO_init->mode == GPIO_MODE_OUTPUT) {
    SET_BITS(GPIOx->OUTPUT_EN, (uint32_t)pin);
  }
  if (GPIO_init->mode == GPIO_MODE_ALTERNATE_FUNCTION) {
    // alternate function not implemented
  }

  if (GPIO_init->pull == GPIO_PULL_UP) {
    SET_BITS(GPIOx->PUE, (uint32_t)pin);
  }
  else {
    CLEAR_BITS(GPIOx->PUE, (uint32_t)pin);
  }

  if (GPIO_init->drive_strength == GPIO_DS_STRONG) {
    SET_BITS(GPIOx->DS, (uint32_t)pin);
  }
  else {
    CLEAR_BITS(GPIOx->DS, (uint32_t)pin);
  }  
}

uint8_t HAL_GPIO_readPin(GPIO_TypeDef *GPIOx, GPIO_PIN pin) {
  return READ_BITS(GPIOx->INPUT_VAL, (uint32_t)pin) ? 1 : 0;
}

void HAL_GPIO_writePin(GPIO_TypeDef *GPIOx, GPIO_PIN pin, uint8_t value) {
  if (value) {
    SET_BITS(GPIOx->OUTPUT_VAL, (uint32_t)pin);
  }
  else {
    CLEAR_BITS(GPIOx->OUTPUT_VAL, (uint32_t)pin);
  }
}

