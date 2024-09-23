#include "time.h"

#ifdef CLINT_BASE
  #include "clint.h"
#endif

uint64_t sys_clk_freq = SYS_CLK_FREQ;
uint64_t mtime_freq = MTIME_FREQ;


__attribute__((weak)) unsigned int sleep(unsigned int seconds) {
  #ifdef CLINT_BASE
    uint64_t target_tick = CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) + (seconds * mtime_freq);
    while (CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) < target_tick) {
      asm volatile("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}

__attribute__((weak)) int msleep(useconds_t msec) {
  #ifdef CLINT_BASE
    uint64_t target_tick = CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) + ((msec * mtime_freq) / 1000);
    while (CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) < target_tick) {
      asm volatile("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}

__attribute__((weak)) int usleep(useconds_t usec) {
  #ifdef CLINT_BASE
    uint64_t target_tick = CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) + ((usec * mtime_freq) / 1000000);
    while (CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) < target_tick) {
      asm volatile("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}
