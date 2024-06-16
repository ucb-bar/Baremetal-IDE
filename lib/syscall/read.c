#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

__attribute__((weak)) ssize_t _read(int file, void *ptr, size_t len) {
  return -1;
}
