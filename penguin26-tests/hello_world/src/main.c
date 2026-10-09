/*
 * main.c - Hello-world bring-up test for the penguin26 chip.
 *
 * Prints the hart ID and does one Atlas CSR access as an MMIO smoke test.
 * On PLATFORM=SIMS stdio goes over HTIF; on PLATFORM=CHIP it goes to UART0, which is
 * initialized here (the default divisor isn't right for a 500 MHz bus clock).
 * Returns 0 (sim exit code 0 = pass).
 */
#include "main.h"

void app_init() {
#if defined(TERMINAL_DEVICE_UART0)
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
#endif
  printf("In the init function.\n");
}

int app_main() {
  uint64_t mhartid = READ_CSR("mhartid");
  printf("Hello world from hart %lu!\n", (unsigned long)mhartid);

  /* The Atlas CSR cycle counter free-runs (CSRFile.scala: reg_cycle := reg_cycle + 1). */
  uint32_t c0 = atlas_get_cycles(ATLAS);
  uint32_t c1 = atlas_get_cycles(ATLAS);
  printf("Atlas CSR cycle counter: %u -> %u\n", (unsigned)c0, (unsigned)c1);
  if (c1 == c0) {
    printf("Atlas cycle counter did not advance\n");
    return 1;
  }
  printf("In the main function.\n");
  return 0;
}

int main(void) {
  app_init();
  return app_main();
}
