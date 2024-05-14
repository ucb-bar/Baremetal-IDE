#include <stdint.h>

#include "rv_atomic.h"
#include "spinlock.h"
#include "htif.h"

volatile uint64_t tohost __attribute__ ((section (".htif")));
volatile uint64_t fromhost __attribute__ ((section (".htif")));

static spinlock_t htif_lock = SPINLOCK_INIT;

long htif_syscall(uint64_t a0, uint64_t a1, uint64_t a2, unsigned long n) {
  volatile uint64_t buf[8];
  uint64_t sc;

  buf[0] = n;
  buf[1] = a0;
  buf[2] = a1;
  buf[3] = a2;

  sc = HTIF_TOHOST(0, 0, (uintptr_t)&buf);
  spin_lock(&htif_lock);
  wmb();
  tohost = sc;
  while (fromhost == 0);
  fromhost = 0;
  spin_unlock(&htif_lock);

  rmb();
  return buf[0];
}

//   char *ptr = "Hello, World!\n";
//   size_t len = 14;
//   int fd = 0;

//   SYSCALL3(SYS_write, fd, (uintptr_t)ptr, len);

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "fesvr.h"


void __attribute__ ((noreturn)) _exit(int code) {
  while (1) {
    fromhost = 0;
    tohost = (code << 1) | 0x1;
  }
}


#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

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