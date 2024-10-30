#ifndef __SYSCON_POWEROFF_H
#define __SYSCON_POWEROFF_H

#include "metal.h"

#define FINISHER_PASS 0x5555

typedef struct {
  __IO uint32_t poweroff;
} SYSCON_Poweroff_Type;

// default base address is 0x100000

void syscon_poweroff(SYSCON_Poweroff_Type *poweroff);

#endif /* __SYSCON_POWEROFF_H */