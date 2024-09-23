#include <errno.h>

__attribute__((weak)) int _open(const char *path, int flags, int mode) {
  return -1;
}
