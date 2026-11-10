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

/* Silicon and the tethered FireSim images: 115200. Override with -D PENGUIN26_UART_BAUDRATE=<baud>
   (CMake cache variable of the same name, platform/penguin26/CMakeLists.txt). */
#ifndef PENGUIN26_UART_BAUDRATE
#define PENGUIN26_UART_BAUDRATE 115200
#endif

#if defined(PENGUIN26_EXIT_TOHOST)
/*
 * Opt-in (-D PENGUIN26_EXIT_TOHOST=ON): report main()'s return value to the TSI host through tohost,
 * as a PLATFORM=SIMS build does, so a run on the chip + bringup FPGA (or the tethered FireSim image)
 * ends with PASS/FAIL. The host reads tohost straight from the bringup DRAM, behind Shuttle's
 * write-back L1 D$, and Shuttle has no cache-flush instruction, so the dirty line is pushed out by
 * reading enough other lines to evict it (read-only, so nothing is modified).
 */
__IO uint64_t tohost __attribute__((section(".htif"), aligned(64)));
__IO uint64_t fromhost __attribute__((section(".htif"), aligned(64)));

#define PENGUIN26_EVICT_BYTES 0x100000UL  /* 1 MiB: many times the L1 D$ */

static void penguin26_evict_l1(void) {
  for (uintptr_t a = DRAM_BASE; a < DRAM_BASE + PENGUIN26_EVICT_BYTES; a += 64) {
    (void)*(volatile uint64_t *)a;
  }
  asm volatile ("fence" ::: "memory");
}
#endif

int __real_main(int argc, char **argv, char **envp);

int __wrap_main(int argc, char **argv, char **envp) {
  UART_InitType UART_init_config;
  UART_init_config.baudrate = PENGUIN26_UART_BAUDRATE;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
  int rc = __real_main(argc, argv, envp);
#if defined(PENGUIN26_EXIT_TOHOST)
  /* Let the UART drain first: the host may stop the run as soon as it sees tohost. With
     txcnt = 1, TXWM is pending once the TX FIFO is empty; then wait out the last character
     (12 bit times covers start + 8 data + 2 stop bits). */
  UART0->TXCTRL = (UART0->TXCTRL & ~(0x7U << UART_TXCTRL_TXCNT_POS)) | (1U << UART_TXCTRL_TXCNT_POS);
  while ((UART0->IP & UART_IP_TXWM_MSK) == 0) {
  }
  uint64_t start;
  asm volatile ("rdcycle %0" : "=r"(start));
  for (uint64_t now = start; now - start < 12ULL * (UART0->DIV + 1);) {
    asm volatile ("rdcycle %0" : "=r"(now));
  }
  tohost = ((uint64_t)rc << 1) | 1;
  asm volatile ("fence" ::: "memory");
  penguin26_evict_l1();
#endif
  return rc;
}

#endif
