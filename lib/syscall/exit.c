
#ifdef DEVICE_TERMINAL_HTIF
  #include "htif.h"
#endif
#ifdef DEVICE_TERMINAL_SYSCON_POWEROFF
  #include "poweroff.h"
#endif

__attribute__((weak, noreturn)) void _exit(int code) {
  // while (1) {
    #if defined(HTIF)
      *(HTIF->fromhost) = 0;
      *(HTIF->tohost) = (code << 1) | 0x1;
    #endif
    #if defined(SYSCON_POWEROFF)
      syscon_poweroff();
    #endif
  // }
  while (1)
  {
    // nop
    asm volatile ("nop");
  }
  
}
