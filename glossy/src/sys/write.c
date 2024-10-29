#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#if defined(TERMINAL_DEVICE_HTIF)
  #include "htif.h"
#endif
#if defined(TERMINAL_DEVICE_UART0) || defined(TERMINAL_DEVICE_NS16550A)
  #include "uart.h"
#endif

__attribute__((weak)) ssize_t _write(int fd, const void *ptr, size_t len) {
  #if defined(TERMINAL_DEVICE_HTIF)
    htif_syscall(fd, (uintptr_t)ptr, len, FESVR_write);
    return len;
  #elif defined(TERMINAL_DEVICE_UART0)
    uart_transmit((UART_Type *)UART0_BASE, (uint8_t *)ptr, len, 100);
    return len;
  #elif defined(TERMINAL_DEVICE_NS16550A)
    ns16550a_puts((uint8_t *)ptr, len);
    return len;
  #else
    return -1;
    #warning "No UART peripheral found. Write function is not available."
  #endif
}

__attribute__((weak)) int __io_putchar(int ch) {
  return _write(0, &ch, 1);
}
