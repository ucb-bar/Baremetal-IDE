/*
 * penguin26 TACIT trace register layouts.
 *
 * The shared driver/rocket-chip/l_trace_encoder types do not match this chip's TACIT build:
 *  - the encoder controller has one extra register after the common block (stall, 64-bit at 0x28);
 *    its first fields match LTraceEncoderType, so the l_trace_encoder helpers still work on it;
 *  - the trace sink DMA has a new layout (no flush/done handshake), so LTraceSinkDmaType and
 *    l_trace_sink_dma_read() must NOT be used on it.
 *
 * Sources: [REGMAP] chipyard.harness.TestHarness.EE290SimConfig.0x3000000.0.regmap.json and
 * .0x3010000.0.regmap.json; [RTL] rocket-chip trace/TraceEncoderController.scala (d4796e7),
 * tacit TraceSinkDMA.scala regmap (4b18ba9).
 */
#ifndef __HAL_TRACE_H
#define __HAL_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "metal.h"

typedef struct {
  __IO uint32_t CTRL;           // 0x00 control (2 bits)
  __I  uint32_t IMPL;           // 0x04 impl
  uint32_t      RESERVED0[6];   // 0x08-0x1C
  __IO uint32_t TARGET;         // 0x20 target (8 bits)
  __IO uint32_t BP_MODE;        // 0x24 branch predictor mode
  __I  uint64_t STALL;          // 0x28 trace encoder stall cycle count
  // No SYNC_INTERVAL: at rocket-chip d4796e7 the controller still has a trace_sync_interval register
  // (drives io.control.sync_interval, reset 0) but it is no longer in the regmap, so software cannot
  // set it. At 9a8d4ed it was RW at 0x30.
} Penguin26TraceEncoder_Type;

typedef struct {
  __IO uint64_t DMA_START_ADDR; // 0x00 DMA start address
  __I  uint64_t ADDR_COUNTER;   // 0x08 bytes written to memory to date
  __IO uint64_t MAX_SIZE;       // 0x10 max bytes before overflow
  __IO uint32_t RESET;          // 0x18 soft reset (1 bit)
  __IO uint32_t MODE;           // 0x1C 0 = overflow mode, 1 = ring-buffer mode
  __IO uint32_t WRAP_COUNT;     // 0x20 times the address counter wrapped
} Penguin26TraceSinkDma_Type;

#ifndef __cplusplus
#include <stddef.h>
_Static_assert(offsetof(Penguin26TraceEncoder_Type, TARGET) == 0x20, "trace encoder layout");
_Static_assert(offsetof(Penguin26TraceEncoder_Type, STALL) == 0x28, "trace encoder layout");
_Static_assert(offsetof(Penguin26TraceSinkDma_Type, RESET) == 0x18, "trace sink DMA layout");
_Static_assert(offsetof(Penguin26TraceSinkDma_Type, WRAP_COUNT) == 0x20, "trace sink DMA layout");
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_TRACE_H */
