#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef DEVICE_TERMINAL_HTIF
  #include "htif.h"
#endif
#ifdef DEVICE_TERMINAL_UART0
  #include "uart.h"
#endif

__attribute__((weak)) ssize_t _write(int fd, const void *ptr, size_t len) {
  #if defined(HTIF)
    HTIF_syscall(fd, (uintptr_t)ptr, len, FESVR_write);
    return len;
  #elif defined(UART0)
    UART_transmit(UART0, (uint8_t *)ptr, len, 100);
    return len;
  #else
    return -1;
    #warning "No UART peripheral found. Write function is not available."
  #endif
}

__attribute__((weak)) int __io_putchar(int ch) {
  return _write(0, &ch, 1);
}
