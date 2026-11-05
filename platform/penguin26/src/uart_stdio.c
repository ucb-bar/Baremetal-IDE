/*
 * uart_stdio.c - PLATFORM=CHIP only: set up UART0 before main() so printf works in every program.
 *
 * On PLATFORM=CHIP glossy sends stdio to UART0 (TERMINAL_DEVICE_UART0), and the UART must be
 * initialized with uart_init() before it transmits anything (Baremetal-IDE lab). hello_world and
 * atlas_example do this themselves in app_init(); the generated Atlas tests and the Saturn benchmarks
 * (whose main() lives in generators/saturn) do not. platform/penguin26/CMakeLists.txt links
 * PLATFORM=CHIP programs with -Wl,--wrap=main, so glossy's crt0 calls __wrap_main() below, which
 * initializes UART0 and then runs the program's own main(). Calling uart_init() again from app_init()
 * is harmless.
 *
 * PLATFORM=SIMS: this file compiles to nothing and no --wrap is added, so SIMS binaries are unchanged.
 */
#include "chip_config.h"

#if defined(TERMINAL_DEVICE_UART0)

#define PENGUIN26_UART_BAUDRATE 115200

int __real_main(int argc, char **argv, char **envp);

int __wrap_main(int argc, char **argv, char **envp) {
  UART_InitType UART_init_config;
  UART_init_config.baudrate = PENGUIN26_UART_BAUDRATE;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
  return __real_main(argc, argv, envp);
}

#endif
