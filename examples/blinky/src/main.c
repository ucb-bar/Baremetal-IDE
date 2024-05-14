#include <stdio.h>
#include <stdint.h>

#include "gpio.h"

#define GPIOA                   ((GPIO_TypeDef *)0x10012000U)

int main(void) {
  // printf("Hello world from core 0\n");

  
  CLEAR_BITS(GPIOA->IOF_EN, (uint32_t)GPIO_PIN_5);
  
  CLEAR_BITS(GPIOA->INPUT_EN, (uint32_t)GPIO_PIN_5);
  SET_BITS(GPIOA->OUTPUT_EN, (uint32_t)GPIO_PIN_5);

  SET_BITS(GPIOA->OUTPUT_VAL, (uint32_t)GPIO_PIN_5);

  return 0;
}
