#include <errno.h>
#include <stddef.h>

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

extern char __end[];
static char *curbrk = __end;

__attribute__((weak)) void *_sbrk(ptrdiff_t incr) {
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
