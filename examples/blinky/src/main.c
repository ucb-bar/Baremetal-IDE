#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "fe310.h"

void APP_init();
void APP_main();

uint8_t counter = 0;


void APP_init() {
  GPIO_InitTypeDef GPIO_init_config;
  GPIO_init_config.mode = GPIO_MODE_OUTPUT;
  GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_5);
}

void APP_main() {
  GPIO_writePin(GPIOA, GPIO_PIN_5, counter % 2);
  counter += 1;

  usleep(200000);
  // sleep(1);
}

int main(void) {
  APP_init();
  while (1) {
    APP_main();
  }
  return 0;
}
