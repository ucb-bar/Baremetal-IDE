
#include "fe310.h"


// weak definition of clock frequency
__attribute__((weak)) uint64_t sys_clk_freq = SYS_CLK_FREQ;
__attribute__((weak)) uint64_t mtime_freq = MTIME_FREQ;
