#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fe310.h"

void APP_init();
void APP_main();

uint8_t counter;

ssize_t _write(int fd, const void *ptr, size_t len) {
  UART_transmit(UART0, (const uint8_t *)ptr, len, 100);
  return len;
}

int __io_putchar(int ch) {
  return _write(0, &ch, 1);
}

void APP_init() {
  // set up UART registers
  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  UART_init(UART0, &UART_init_config);

  GPIO_InitTypeDef GPIO_init_config;
  GPIO_init_config.mode = GPIO_MODE_OUTPUT;
  GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_5);
  
  GPIO_init_config.mode = GPIO_MODE_ALTERNATE_FUNCTION_0;
  GPIO_init(GPIOA, &GPIO_init_config, GPIO_PIN_16 | GPIO_PIN_17);
}

void APP_main() {
    
  printf("1. string with newline\n");

  printf("2. string with formatting: %d", counter);
  printf("\n");

  printf("3. string");
  printf("\n");
  
  counter += 1;

  sleep(1);
}

int main(void) {
  APP_init();
  while (1) {
    APP_main();
  }
  return 0;
}
