#include <stdint.h>

uint32_t *GPIOA_OUTPUT_VAL = (uint32_t*) 0x1001000CUL;
uint32_t *GPIOA_OUTPUT_EN  = (uint32_t*) 0x10010008UL;
uint32_t *CLINT_MTIME      = (uint32_t*) 0x0200BFF8UL;

void delay(unsigned int ticks) {
  unsigned int mtime_start;
  while (*CLINT_MTIME - mtime_start < ticks);
}

void main() {
    int counter = 0;

    *GPIOA_OUTPUT_EN = 0b1;

    while (1) {
        if (counter % 2 == 0) {
            *GPIOA_OUTPUT_VAL = 0b1;
        } else {
            *GPIOA_OUTPUT_VAL = 0b0;
        }
        counter ++;
        delay(4000000);
    }
}

