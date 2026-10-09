/*
 * saturn_compat.c - replacements for the parts of riscv-tests' common/syscalls.c that the
 * Saturn benchmarks rely on (see saturn_compat.h). Behavior follows syscalls.c:
 *   setStats(1): record mcycle and minstret.
 *   setStats(0): store the deltas; syscalls.c printed them after main() returned, as
 *                "mcycle = %d\nminstret = %d\n". glossy has no post-main hook, so they are
 *                printed here, at disable time, with the same text.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "saturn_compat.h"

static inline uint64_t read_mcycle(void) {
  uint64_t v;
  asm volatile ("csrr %0, mcycle" : "=r"(v));
  return v;
}

static inline uint64_t read_minstret(void) {
  uint64_t v;
  asm volatile ("csrr %0, minstret" : "=r"(v));
  return v;
}

static uint64_t start_cycle, start_instret;

void setStats(int enable) {
  /* same read order as syscalls.c: mcycle first, then minstret */
  uint64_t c = read_mcycle();
  uint64_t i = read_minstret();
  if (enable) {
    start_cycle = c;
    start_instret = i;
  } else {
    printf("mcycle = %ld\n", (long)(c - start_cycle));
    printf("minstret = %ld\n", (long)(i - start_instret));
  }
}

void printstr(const char *s) {
  write(1, s, strlen(s));
}

void printhex(uint64_t x) {
  char str[17];
  for (int i = 0; i < 16; i++) {
    str[15 - i] = (x & 0xF) + ((x & 0xF) < 10 ? '0' : 'a' - 10);
    x >>= 4;
  }
  str[16] = 0;
  printstr(str);
}

/*
 * Trap behavior. riscv-tests' syscalls.c defines
 *     uintptr_t __attribute__((weak)) handle_trap(...) { tohost_exit(1337); }
 * so ANY trap ends the benchmark with exit code 1337, by writing tohost = (1337 << 1) | 1
 * directly (no HTIF syscall). glossy's default handlers spin forever
 * (glossy/src/trap/trap.c: `while (1) {}`), which would turn a trap into a max-cycles TIMEOUT.
 * Override glossy's weak trap_handler with the original semantics. No printf here: a trapping
 * program's state may be inconsistent, and an HTIF syscall from it crashed fesvr ("bad syscall")
 * during testing. The cause is kept in globals for debugging (readable from a waveform/dump).
 */
volatile uintptr_t saturn_compat_trap_cause, saturn_compat_trap_epc, saturn_compat_trap_tval;

uintptr_t trap_handler(uintptr_t m_epc, uintptr_t m_cause, uintptr_t m_tval, uintptr_t regs[32]) {
  (void)regs;
  saturn_compat_trap_cause = m_cause;
  saturn_compat_trap_epc = m_epc;
  saturn_compat_trap_tval = m_tval;
  _exit(1337);   /* glossy _exit: tohost = (code << 1) | 1 under TERMINAL_DEVICE_HTIF */
}
