#include "time.h"

#ifdef CLINT_BASE
  #include "clint.h"
#endif


__attribute__((weak)) unsigned int sleep(unsigned int seconds) {
  #ifdef CLINT_BASE
    uint64_t target_tick = CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) + (seconds * MTIME_FREQUENCY);
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
    uint64_t target_tick = CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) + ((msec * MTIME_FREQUENCY) / 1000);
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
    uint64_t target_tick = CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) + ((usec * MTIME_FREQUENCY) / 1000000);
    while (CLINT_getTime((CLINT_TypeDef *)CLINT_BASE) < target_tick) {
      asm volatile("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}
