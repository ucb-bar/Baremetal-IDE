
#ifndef __QEMUVIRT_H
#define __QEMUVIRT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rv.h>

/**
 * System Clock Configuration
 */
#define HXTAL_FREQ          16000000                        /** crystal or external clock frequency in Hz */
#define SYS_CLK_FREQ        HXTAL_FREQ                      /** system clock frequency in Hz */
#define MTIME_TIMEBASE      1                               /** tick per milliseconds */
#define MTIME_FREQ          100

// weak definition of clock frequency
__attribute__((weak)) uint64_t sys_clk_freq = SYS_CLK_FREQ;
__attribute__((weak)) uint64_t mtime_freq = MTIME_FREQ;

#ifdef __cplusplus
}
#endif

#endif /* __QEMUVIRT_H */
