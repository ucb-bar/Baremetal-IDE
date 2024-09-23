#ifndef __SYSCON_POWEROFF_H
#define __SYSCON_POWEROFF_H

#include "hal.h"

#define SYSCON_POWEROFF 0x100000
#define FINISHER_PASS 0x5555

void syscon_poweroff(void);

#endif /* __SYSCON_POWEROFF_H */