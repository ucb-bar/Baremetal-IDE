#include "poweroff.h"

void syscon_poweroff(void) {
   volatile uint32_t *ptr = (volatile uint32_t *) SYSCON_POWEROFF;
   *ptr = FINISHER_PASS;
}