#ifndef __LL_RCC_H
#define __LL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rv_common.h"


typedef struct {
  __IO uint32_t TILE0_RESET;      /** RocketTile 0 */   // 0x00
  __IO uint32_t TILE1_RESET;      /** RocketTile 1 */   // 0x04
  __IO uint32_t TILE2_RESET;      /** RocketTile 2 */   // 0x08
  __IO uint32_t TILE3_RESET;      /** RocketTile 3 */   // 0x0C
  __IO uint32_t TILE4_RESET;      /** SaturnTile 0 */   // 0x10
  uint32_t RESERVED0[1021];
  __IO uint32_t TILE0_CLKGATE;    /** RocketTile 0 */   // 0x1008
  __IO uint32_t TILE1_CLKGATE;    /** RocketTile 1 */   // 0x100C
  __IO uint32_t TILE2_CLKGATE;    /** RocketTile 2 */   // 0x1010
  __IO uint32_t TILE3_CLKGATE;    /** RocketTile 3 */   // 0x1014
  __IO uint32_t TILE4_CLKGATE;    /** SaturnTile 4 */   // 0x1018
  uint32_t RESERVED1[1017];
  __IO uint32_t CLK_SEL;          /** Selects the system clock. 0: External slow clock, 1: PLL clock */
  __IO uint32_t DEBUG_CLK_EN;                           // 0x2004
  __IO uint32_t DEBUG_CLK_SEL;                          // 0x2008
  __IO uint32_t DEBUG_CLK_DIV;                          // 0x200C
  uint32_t RESERVED2[1021];
  __IO uint32_t FBUS_CLK_DIV;                           // 0x3000
  __IO uint32_t UNCORE_CLK_DIV;                         // 0x3004
  __IO uint32_t TILE0_CLK_DIV;                          // 0x3008
  __IO uint32_t TILE1_CLK_DIV;                          // 0x300C
  __IO uint32_t TILE2_CLK_DIV;                          // 0x3010
  __IO uint32_t TILE3_CLK_DIV;                          // 0x3014
  __IO uint32_t TILE4_CLK_DIV;                          // 0x3018
} RCC_TypeDef;


#ifdef __cplusplus
}
#endif

#endif /* __LL_RCC_H */