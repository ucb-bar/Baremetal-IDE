
#include <stdio.h>
#include "rv.h"
#include "marchid.h"

// EDIT THIS
static size_t n_cores = 8;

/**
 * Barrier implementation
 * 
 * This is a simple barrier implementation that can be used to synchronize
 * multiple threads. It is implemented as a spinlock barrier, which means
 * that threads will busy-wait until all threads have reached the barrier.
 */
static void __attribute__((noinline)) barrier() {
  static volatile int sense;
  static volatile int count;
  static __thread int threadsense;

  __sync_synchronize();

  threadsense = !threadsense;
  if (__sync_fetch_and_add(&count, 1) == n_cores-1) {
    count = 0;
    sense = threadsense;
  }
  else while (sense != threadsense) {}

  __sync_synchronize();
}

void __main(void) {
  size_t mhartid = READ_CSR("mhartid");

  if (mhartid >= n_cores) while (1);

  const char* march = get_machine_architecture(READ_CSR("marchid"));
  for (size_t i = 0; i < n_cores; i++) {
    if (mhartid == i) {
      printf("Hello world from core %lu, a %s\n", mhartid, march);
    }
    barrier();
  }

  // Spin if not core 0
  if (mhartid > 0) while (1);
}

int main(void) {
  __main();
  return 0;
}