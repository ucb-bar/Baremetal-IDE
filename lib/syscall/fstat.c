#include <errno.h>
#include <sys/stat.h>

__attribute__((weak)) int _fstat(int file, struct stat *st) {
  return -1;
}
