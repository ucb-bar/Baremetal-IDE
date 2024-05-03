
#ifndef __SPIKE_H
#define __SPIKE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_arch.h"
#include "rv_common.h"


/**
 * System Clock Configuration
 */
#define HXTAL_FREQ          16000000                        /** crystal or external clock frequency in Hz */
#define SYS_CLK_FREQ        HXTAL_FREQ                      /** system clock frequency in Hz */
#define MTIME_TIMEBASE      1                               /** tick per milliseconds */
#define MTIME_FREQ          100

#ifdef __cplusplus
}
#endif

#endif /* __SPIKE_H */
