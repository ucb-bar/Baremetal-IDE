#include <stdint.h>
#include <stddef.h>

#include "htif.h"
#include "fesvr.h"

__attribute__((weak)) ssize_t _write(int fd, const void *ptr, size_t len) {
  SYSCALL3(FESVR_write, fd, (uintptr_t)ptr, len);
  return len;
}
