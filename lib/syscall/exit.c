
#ifdef DEVICE_TERMINAL_HTIF
  #include "htif.h"
#endif

__attribute__((weak, noreturn)) void _exit(int code) {
  while (1) {
    #if defined(HTIF)
      *(HTIF->fromhost) = 0;
      *(HTIF->tohost) = (code << 1) | 0x1;
    #endif
  }
}
