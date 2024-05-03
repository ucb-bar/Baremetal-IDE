#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "syscall.h"
#include "main.h"

ssize_t _write(int fd, const void *ptr, size_t len) {
  HAL_UART_transmit(UART0, (const uint8_t *)ptr, len, 100);
  return len;
}
