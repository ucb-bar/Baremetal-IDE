#include "syscalls.h"

ssize_t _write(int fd, const void *ptr, size_t len) {
  HAL_UART_transmit(UART0, (const uint8_t *)ptr, len, 100);
  return 0;
}

extern char __end[];
static char *curbrk = __end;

void *_sbrk(ptrdiff_t incr) {
  extern char __heap_end[];
  char *newbrk;
  char *oldbrk;

  oldbrk = curbrk;
  newbrk = oldbrk + incr;
  if (unlikely((newbrk < __end) || (newbrk >= __heap_end))) {
    errno = ENOMEM;
    return (void *)(-1);
  }

  curbrk = newbrk;
  return oldbrk;
}

int _fstat(int file, struct stat *st) {
  errno = EBADF;
  return -1;
}
