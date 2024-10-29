#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "metal.h"


/* Peripheral Struct Definition */
typedef struct {
  __I  uint32_t INPUT_VAL;                      /** pin value */
  __IO uint32_t INPUT_EN;                       /** pin input enable */
  __IO uint32_t OUTPUT_EN;                      /** Pin output enable */
  __IO uint32_t OUTPUT_VAL;                     /** Output value */
  __IO uint32_t PUE;                            /** Internal pull-up enable */
  __IO uint32_t DS;                             /** Pin drive strength */
  __IO uint32_t RISE_IE;                        /** Rise interrupt enable */
  __IO uint32_t RISE_IP;                        /** Rise interrupt pending */
  __IO uint32_t FALL_IE;                        /** Fall interrupt enable */
  __IO uint32_t FALL_IP;                        /** Fall interrupt pending */
  __IO uint32_t HIGH_IE;                        /** High interrupt pending */
  __IO uint32_t HIGH_IP;                        /** High interrupt pending */
  __IO uint32_t LOW_IE;                         /** Low interrupt pending */
  __IO uint32_t LOW_IP;                         /** Low interrupt pending */
  __IO uint32_t IOF_EN;                         /** Hardware IO function enable */
  __IO uint32_t IOF_SEL;                        /** Hardware IO function selection */
  __IO uint32_t OUT_XOR;                        /** Output XOR (invert) */
} GPIO_Type;


typedef enum {
  GPIO_PIN_0  = 0b00000000000000000000000000000001U,
  GPIO_PIN_1  = 0b00000000000000000000000000000010U,
  GPIO_PIN_2  = 0b00000000000000000000000000000100U,
  GPIO_PIN_3  = 0b00000000000000000000000000001000U,
  GPIO_PIN_4  = 0b00000000000000000000000000010000U,
  GPIO_PIN_5  = 0b00000000000000000000000000100000U,
  GPIO_PIN_6  = 0b00000000000000000000000001000000U,
  GPIO_PIN_7  = 0b00000000000000000000000010000000U,
  GPIO_PIN_8  = 0b00000000000000000000000100000000U,
  GPIO_PIN_9  = 0b00000000000000000000001000000000U,
  GPIO_PIN_10 = 0b00000000000000000000010000000000U,
  GPIO_PIN_11 = 0b00000000000000000000100000000000U,
  GPIO_PIN_12 = 0b00000000000000000001000000000000U,
  GPIO_PIN_13 = 0b00000000000000000010000000000000U,
  GPIO_PIN_14 = 0b00000000000000000100000000000000U,
  GPIO_PIN_15 = 0b00000000000000001000000000000000U,
  GPIO_PIN_16 = 0b00000000000000010000000000000000U,
  GPIO_PIN_17 = 0b00000000000000100000000000000000U,
  GPIO_PIN_18 = 0b00000000000001000000000000000000U,
  GPIO_PIN_19 = 0b00000000000010000000000000000000U,
  GPIO_PIN_20 = 0b00000000000100000000000000000000U,
  GPIO_PIN_21 = 0b00000000001000000000000000000000U,
  GPIO_PIN_22 = 0b00000000010000000000000000000000U,
  GPIO_PIN_23 = 0b00000000100000000000000000000000U,
  GPIO_PIN_24 = 0b00000001000000000000000000000000U,
  GPIO_PIN_25 = 0b00000010000000000000000000000000U,
  GPIO_PIN_26 = 0b00000100000000000000000000000000U,
  GPIO_PIN_27 = 0b00001000000000000000000000000000U,
  GPIO_PIN_28 = 0b00010000000000000000000000000000U,
  GPIO_PIN_29 = 0b00100000000000000000000000000000U,
  GPIO_PIN_30 = 0b01000000000000000000000000000000U,
  GPIO_PIN_31 = 0b10000000000000000000000000000000U,
  GPIO_PIN_ALL = 0b11111111111111111111111111111111U,
} GPIO_Pin;

typedef enum {
  GPIO_MODE_INPUT,
  GPIO_MODE_OUTPUT,
  GPIO_MODE_INOUT,
  GPIO_MODE_ALTERNATE_FUNCTION_0,
  GPIO_MODE_ALTERNATE_FUNCTION_1
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
} GPIO_InitType;


void gpio_init(GPIO_Type *GPIOx, GPIO_InitType *GPIO_init, GPIO_Pin pin);

uint8_t gpio_read_pin(GPIO_Type *GPIOx, GPIO_Pin pin);

void gpio_write_pin(GPIO_Type *GPIOx, GPIO_Pin pin, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */
