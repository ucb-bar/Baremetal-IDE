/**
 * @file hal_sysctrl.h
 * @brief Chipyard system-control registers on the penguin26 chip: boot-address register,
 *        tile clock gater, tile reset setter.
 *
 * Layouts come from the register maps generated for EE290SimConfig
 * (`make -C sims/vcs CONFIG=EE290SimConfig verilog` in Chipyard):
 *   chipyard.harness.TestHarness.EE290SimConfig.0x1000.0.regmap.json   boot-address-reg:
 *       one 64-bit field at byte offset 0x0 (8 x 8-bit regfields 0..56)
 *   chipyard.harness.TestHarness.EE290SimConfig.0x100000.0.regmap.json clock-gater:
 *       1-bit field at byte offset 0x0 (one per tile; this chip has 1 gated tile)
 *   chipyard.harness.TestHarness.EE290SimConfig.0x110000.0.regmap.json tile-reset-setter:
 *       1-bit field at byte offset 0x0 (one per tile)
 * Field meaning, from generators/chipyard/src/main/scala/clocking/:
 *   TileClockGater.scala:31-45   one AsyncResetRegVec(w=1, init=1) per clock sink at i*4; the
 *                                bit drives ClockGate(...) enable, so 1 = clock on (reset value).
 *   TileResetSetter.scala:28-39  one AsyncResetRegVec(w=1) per tile at i*4; 1 = tile held in reset.
 * Writing either register for hart 0 from hart 0 stops the caller.
 */
#ifndef __HAL_SYSCTRL_H
#define __HAL_SYSCTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "metal.h"

typedef struct {
  __IO uint64_t BOOT_ADDR;  /* 0x00: address the bootrom jumps to after reset/MSIP */
} BootAddrReg_Type;

typedef struct {
  __IO uint32_t TILE_EN[1]; /* 0x00: bit 0 = tile 0 clock enable */
} ClockGater_Type;

typedef struct {
  __IO uint32_t TILE_RESET[1]; /* 0x00: bit 0 = tile 0 reset */
} TileResetSetter_Type;

static inline uint64_t sysctrl_get_boot_addr(BootAddrReg_Type *reg) { return reg->BOOT_ADDR; }
static inline void sysctrl_set_boot_addr(BootAddrReg_Type *reg, uint64_t addr) { reg->BOOT_ADDR = addr; }

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SYSCTRL_H */
