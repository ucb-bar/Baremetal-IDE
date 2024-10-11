
#ifdef TERMINAL_DEVICE_HTIF
  #include "htif.h"
#endif
#ifdef TERMINAL_DEVICE_SYSCON_POWEROFF
  #include "poweroff.h"
#endif

__attribute__((weak, noreturn)) void _exit(int code) {
  #if defined(TERMINAL_DEVICE_HTIF)
    *(HTIF->fromhost) = 0;
    *(HTIF->tohost) = (code << 1) | 0x1;
  #endif
  
  #if defined(SYSCON_POWEROFF)
    syscon_poweroff();
  #endif

  while (1) {
    asm volatile ("nop");
  }
}
