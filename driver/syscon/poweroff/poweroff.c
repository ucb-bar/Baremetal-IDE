#include "poweroff.h"

void syscon_poweroff(SYSCON_Poweroff_Type *poweroff) {
   poweroff->poweroff = FINISHER_PASS;
}
