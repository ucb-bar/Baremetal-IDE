#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

__attribute__((weak)) ssize_t _write(int fd, const void *ptr, size_t len) {
  #ifdef UART0
  UART_transmit(UART0, (uint8_t *)ptr, len, 100);
  #endif
  return -1;
}
