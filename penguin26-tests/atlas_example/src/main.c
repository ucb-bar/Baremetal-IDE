/*
 * main.c - atlas_example: the smallest complete Atlas NPU test on penguin26.
 *
 * Copy this directory to start a new test. It shows the whole host-side flow:
 *   1. stop the Atlas core and clear its debug registers,
 *   2. write an Atlas program into Atlas IMEM (at runtime; the CPU program itself runs from DRAM),
 *   3. read IMEM back to check the load,
 *   4. start the core and wait until the program reports "done" in DBG0,
 *   5. check the result and print PASS/FAIL.
 *
 * The Atlas program (src/sum.S) adds 1..10 and leaves the result (55) in DBG1.
 * The return value of main() is the test verdict: 0 = pass, nonzero = fail. In RTL simulation
 * glossy hands it to the simulator through HTIF, so the sim itself passes or fails.
 */
#include "main.h"

/* src/sum.S, encoded with generators/atlas-npu/baremetal/assembler.py (one 32-bit word each). */
static const uint32_t atlas_program[] = {
  0x00000093,  /*       ADDI  x1, x0, 0        sum = 0          */
  0x00a00113,  /*       ADDI  x2, x0, 10       i = 10           */
  0x002080b3,  /* loop: ADD   x1, x1, x2       sum += i         */
  0xfff10113,  /*       ADDI  x2, x2, -1       i--              */
  0xfe011ee3,  /*       BNE   x2, x0, loop                      */
  0x00000013,  /*       NOP                    delay slot 1     */
  0x00000013,  /*       NOP                    delay slot 2     */
  0xc1109073,  /*       CSRRW x0, 0xC11, x1    DBG1 = sum       */
  0x00100193,  /*       ADDI  x3, x0, 1                         */
  0xc1019073,  /*       CSRRW x0, 0xC10, x3    DBG0 = 1 (done)  */
  0x00000073,  /*       ECALL                  halt             */
};
#define ATLAS_PROGRAM_WORDS (sizeof(atlas_program) / sizeof(atlas_program[0]))

#define EXPECTED_SUM 55U  /* 1 + 2 + ... + 10 */

void app_init() {
#if defined(TERMINAL_DEVICE_UART0)
  /* PLATFORM=CHIP: stdio goes to UART0, whose divisor comes from SYS_CLK_FREQ. */
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
#endif
}

int app_main() {
  int fail = 0;

  /* 1-3: load the program into IMEM and check it (ATLAS / ATLAS_IMEM come from chip_config.h). */
  atlas_reset_for_load(ATLAS);
  if (atlas_load_program(ATLAS_IMEM, atlas_program, ATLAS_PROGRAM_WORDS) != 0) {
    printf("Program does not fit in Atlas IMEM\n");
    return 1;
  }
  if (atlas_verify_program(ATLAS_IMEM, atlas_program, ATLAS_PROGRAM_WORDS, 1) != 0) {
    printf("IMEM readback mismatch\n");
    return 1;
  }

  /* 4: run it. The program writes DBG0 = 1 when it is done, then halts with ECALL. */
  atlas_start(ATLAS);
  uint32_t done = atlas_wait_dbg0(ATLAS, ATLAS_DEFAULT_POLL_LIMIT);
  int halted = atlas_wait_halted(ATLAS, 1000);
  ATLAS_HaltReason reason = atlas_get_halt_reason(ATLAS);
  atlas_stop(ATLAS);

  /* 5: check everything the program should have left behind. */
  uint32_t sum = atlas_get_dbg1(ATLAS);
  printf("Atlas: dbg0=%u dbg1=%u (expected %u), halted=%d reason=%u, %u instructions\n",
         (unsigned)done, (unsigned)sum, EXPECTED_SUM, halted, (unsigned)reason,
         (unsigned)atlas_get_instret(ATLAS));
  if (done != 1) {
    printf("Atlas program did not finish (dbg0=%u)\n", (unsigned)done);
    fail = 1;
  }
  if (!halted || reason != ATLAS_HALT_ECALL) {
    printf("Atlas core did not halt with ECALL (halted=%d reason=%u)\n", halted, (unsigned)reason);
    fail = 1;
  }
  if (sum != EXPECTED_SUM) {
    printf("Wrong sum: got %u, expected %u\n", (unsigned)sum, EXPECTED_SUM);
    fail = 1;
  }

  printf(fail ? "*** FAILED ***\n" : "*** PASSED ***\n");
  return fail;
}

int main(void) {
  app_init();
  return app_main();
}
