#include <unistd.h>

#include "clint.h"

extern uint64_t mtime_freq;

/**
 * @brief Sleep for given time
 * 
 * This function put the hart to sleep for the given amount of time.
 * 
 * @param __seconds time to sleep
 */
__attribute__((weak)) unsigned sleep(unsigned int __seconds) {
  #ifdef CLINT
  uint64_t target_tick = CLINT_getTime() + (__seconds * mtime_freq);
  while (CLINT_getTime() < target_tick) {
    asm("nop");
  }
  #else
  #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}

__attribute__((weak)) int msleep(useconds_t msec) {
  #ifdef CLINT
  uint64_t target_tick = CLINT_getTime() + ((msec * mtime_freq) / 1000);
  while (CLINT_getTime() < target_tick) {
    asm("nop");
  }
  #else
  #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}

__attribute__((weak)) int usleep(useconds_t usec) {
  #ifdef CLINT
  uint64_t target_tick = CLINT_getTime() + ((usec * mtime_freq) / 1000000);
  while (CLINT_getTime() < target_tick) {
    asm("nop");
  }
  #else
  #warning "No CLINT peripheral found. Delay function is not available."
  #endif

  return 0;
}
