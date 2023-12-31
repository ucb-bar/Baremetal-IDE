#ifndef __HAL_CONF_H
#define __HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


#include "examplechip.h"

/**
 * This section controls which peripheral device is included in the application program.
 * To save the memory space, the unused peripheral device can be commented out.
 */
#include "hal_core.h"
#include "hal_clint.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_plic.h"
#include "hal_uart.h"

/**
 * System Clock Configuration
 */
#define HXTAL_FREQ          50000000                        /** crystal or external clock frequency in Hz */
#define SYS_CLK_FREQ        HXTAL_FREQ                      /** system clock frequency in Hz */
#define MTIME_TIMEBASE      50000                           /** tick per milliseconds */
#define MTIME_FREQ          (SYS_CLK_FREQ / MTIME_TIMEBASE)

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_CONF_H */
