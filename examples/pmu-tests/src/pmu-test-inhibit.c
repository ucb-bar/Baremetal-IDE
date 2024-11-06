#include <stdint.h>
#include "riscv.h"
#include "pmu.h"

#define NUM_LOADS 1000

static volatile uint64_t load_addr[NUM_LOADS];

int main(int argc, char **argv) {
  // enable counter enable
  // PMU_COUNTER_ENABLE(3);

  // configure event3 and reset counter3
  PMU_EVENT_ENABLE(PMU_EVENT(0, LOAD), 3);
  PMU_COUNTER_RESET(3);

  // perform loads
  for (int i = 0; i < NUM_LOADS; i++) {
    volatile uint64_t val = load_addr[i];
  }

  // check event3
  uint64_t counter3_after_0 = PMU_COUNTER_READ(3);
  uint64_t event3_after = READ_CSR("mhpmevent3");


  // enable inhibit
  PMU_INHIBIT_ENABLE(3);

  // perform loads
  for (int i = 0; i < NUM_LOADS; i++) {
    volatile uint64_t val = load_addr[i];
  }

  // check event3
  uint64_t counter3_after_1 = PMU_COUNTER_READ(3);

  // disable inhibit
  PMU_INHIBIT_DISABLE(3);

  // perform loads
  for (int i = 0; i < NUM_LOADS; i++) {
    volatile uint64_t val = load_addr[i];
  }

  // check event3
  uint64_t counter3_after_2 = PMU_COUNTER_READ(3);

  printf("event3 after: %lx\n", event3_after);
  printf("counter3 after loading  : %lld\n", counter3_after_0);
  printf("counter3 after inhibit  : %lld\n", counter3_after_1);
  printf("counter3 after uninhibit: %lld\n", counter3_after_2);
}