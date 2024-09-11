#include "time.h"

#ifdef DEVICE_TIME_CLINT
  #include "clint.h"
#endif

extern uint64_t mtime_frequency;

__attribute__((weak)) unsigned int sleep(unsigned int seconds) {
  #ifdef CLINT_BASE
    int target_tick = CLINT_getTime() + (seconds * mtime_frequency);
    while (CLINT_getTime() < target_tick) {
      asm("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}

__attribute__((weak)) int msleep(useconds_t msec) {
  #ifdef CLINT_BASE
    int target_tick = CLINT_getTime() + ((msec * mtime_frequency) / 1000);
    while (CLINT_getTime() < target_tick) {
      asm("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}

__attribute__((weak)) int usleep(useconds_t usec) {
  #ifdef CLINT_BASE
    int target_tick = CLINT_getTime() + ((usec * mtime_frequency) / 1000000);
    while (CLINT_getTime() < target_tick) {
      asm("nop");
    }
  #else
    #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}
