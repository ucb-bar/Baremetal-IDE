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
#define HXTAL_FREQ     20000000                   // Hz
// #define HXTAL_FREQ    100000000                   // Hz
#define SYS_CLK_FREQ  HXTAL_FREQ / 2              // Hz
#define MTIME_FREQ    (SYS_CLK_FREQ / 100000)     // tick per milliseconds

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_CONF_H */
