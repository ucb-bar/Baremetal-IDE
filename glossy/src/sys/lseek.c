#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

__attribute__((weak)) off_t _lseek(int file, off_t ptr, int dir) {
  return -1;
}
