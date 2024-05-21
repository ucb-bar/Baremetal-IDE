#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arty.h"

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
}

void APP_main() {
  float buffer[14];
  UART_receive(UART0, (uint8_t *)buffer, 14 * sizeof(float), 100);

  buffer[0] += 1.0;
  buffer[1] += 1.0;

  UART_transmit(UART0, (uint8_t *)buffer, 14 * sizeof(float), 100);
  
}

int main(void) {
  APP_init();
  while (1) {
    APP_main();
  }
  return 0;
}
