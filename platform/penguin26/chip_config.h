#ifndef __CHIP_CONFIG_H
#define __CHIP_CONFIG_H

/*
 * Platform configuration for the penguin26 (EE194/290 tapeout) chip (chipyard `EE290SimConfig`):
 * 1x Shuttle core + Saturn vector unit (VLEN 256), Atlas NPU tile, TACIT trace, AbstractConfig
 * peripherals, all buses at 500 MHz.
 *
 * Sources (generated in Chipyard by `make -C sims/vcs CONFIG=EE290SimConfig verilog`, under
 * sims/vcs/generated-src/chipyard.harness.TestHarness.EE290SimConfig/):
 *   [DTS]    chipyard.harness.TestHarness.EE290SimConfig.dts
 *   [MAP]    "Generated Address Map" printed during that elaboration;
 *            the Atlas regions are TL managers without a DTS node, so they only appear there as
 *            "exists, but undescribed by DTS: AddressRange(base, size)"
 *   [REGMAP] chipyard.harness.TestHarness.EE290SimConfig.<base>.regmap.json
 *   [RTL]    generators/atlas-npu/src/main/scala/atlas/scalar/ScalarCore.scala (object AtlasMemMap)
 *
 * Generated (2026-10-06) from the TA-bumped submodule SHAs with no patches: rocket-chip d4796e7,
 * saturn de04946, shuttle a535c02, tacit 4b18ba9, testchipip c807cad, atlas-npu 0079c05. Versus
 * the earlier provisional (patched-RTL) DTS only the TACIT trace nodes changed: see TRACE_* below
 * and hal_trace.h.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "riscv.h"
#include "clint.h"
#include "plic.h"
#include "htif.h"
#include "uart.h"
#include "l_trace_encoder.h"

// ================================
//  Platform Drivers
// ================================
#include "hal_atlas.h"
#include "hal_sysctrl.h"
#include "hal_trace.h"


// ================================
//  System Clock
// ================================
// system clock frequency in Hz
// [DTS] cbus/pbus/sbus/mbus/fbus_clock clock-frequency = <500000000>
// (EE290BaseConfig: With*BusFrequency(500.0), WithHarnessBinderClockFreqMHz(500.0))
// Override for other clocks (e.g. an FPGA build): -DSYS_CLK_FREQ=<Hz> -DMTIME_FREQ=<Hz>
#ifndef SYS_CLK_FREQ
#define SYS_CLK_FREQ   500000000
#endif

// CLINT time base frequency in Hz
// [DTS] cpus/timebase-frequency = <500000>
#ifndef MTIME_FREQ
#define MTIME_FREQ     500000
#endif


// ================================
//  MMIO devices
// ================================
#define DEBUG_CONTROLLER_BASE    0x00000000U  // [DTS] debug-controller@0, size 0x1000
#define BOOTADDR_REG_BASE        0x00001000U  // [DTS] boot-address-reg@1000, size 0x1000
#define ERROR_DEVICE_BASE        0x00003000U  // [DTS] error-device@3000, size 0x1000
#define BOOTROM_BASE             0x00010000U  // [DTS] rom@10000, size 0x10000
#define ATLAS_IMEM_BASE          0x00020000U  // [MAP] AddressRange(0x20000, 0x21000) = IMEM (128 KiB) + CSR (4 KiB); [RTL] AtlasMemMap.IMEM_BASE
#define ATLAS_IMEM_SIZE          0x00020000U  // [RTL] AtlasMemMap.IMEM_SIZE = 128 KiB
#define ATLAS_CSR_BASE           0x00040000U  // [MAP] see above; [RTL] AtlasMemMap.CSR_BASE, window 0x1000
#define CLOCK_GATER_BASE         0x00100000U  // [DTS] clock-gater@100000, size 0x1000
#define TILE_RESET_SETTER_BASE   0x00110000U  // [DTS] tile-reset-setter@110000, size 0x1000
#define CLINT_BASE               0x02000000U  // [DTS] clint@2000000, size 0x10000
#define TRACE_ENCODER_BASE       0x03000000U  // [DTS] trace-encoder-controller0@3000000 ("ucbbar,trace"), size 0x10000
#define TRACE_SINK_DMA_BASE      0x03010000U  // [DTS] trace-sink-dma0@3010000 ("ucbbar,tracesinkdma"), size 0x1000
#define SCRATCHPAD_BASE          0x08000000U  // [DTS] memory@8000000, size 0x10000 (status = "disabled")
#define SCRATCHPAD_SIZE          0x00010000U
#define PLIC_BASE                0x0C000000U  // [DTS] interrupt-controller@c000000, size 0x4000000
#define UART_BASE                0x10020000U  // [DTS] serial@10020000 ("sifive,uart0"), size 0x1000
#define ATLAS_VMEM_BASE          0x20000000U  // [MAP] AddressRange(0x20000000, 0x180000); [RTL] AtlasMemMap.VMEM_BASE
#define ATLAS_VMEM_SIZE          0x00180000U  // [RTL] AtlasMemMap.VMEM_SIZE = 1.5 MiB (6 banks)
#define DRAM_BASE                0x80000000UL // [DTS] memory@80000000
#define DRAM_SIZE                0x1000000000UL // [DTS] reg size 0x10_0000_0000 (WithExtMemSize)

// Where the Atlas baremetal tests place their DRAM preload/check data
// (generators/atlas-npu/baremetal/assembly/*.S `# @DRAM_BASE`). The linker scripts keep every
// program section below this address.
#define ATLAS_TEST_DRAM_BASE     0x90000000UL

#define CLINT                    ((CLINT_Type *)CLINT_BASE)
#define PLIC                     ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                  ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))

#define UART0_BASE               (UART_BASE)
#define UART0                    ((UART_Type *)UART0_BASE)

#define ATLAS                    ((ATLAS_Type *)ATLAS_CSR_BASE)
#define ATLAS_IMEM               ((volatile uint32_t *)ATLAS_IMEM_BASE)
#define ATLAS_VMEM               ((volatile uint8_t *)ATLAS_VMEM_BASE)

#define BOOTADDR_REG             ((BootAddrReg_Type *)BOOTADDR_REG_BASE)
#define CLOCK_GATER              ((ClockGater_Type *)CLOCK_GATER_BASE)
#define TILE_RESET_SETTER        ((TileResetSetter_Type *)TILE_RESET_SETTER_BASE)

// TRACE_ENCODER0 keeps the shared l_trace_encoder type (its fields are a prefix of this chip's
// encoder); PENGUIN26_TRACE_ENCODER0 also exposes STALL.
#define TRACE_ENCODER0           ((LTraceEncoderType *)TRACE_ENCODER_BASE)
#define PENGUIN26_TRACE_ENCODER0 ((Penguin26TraceEncoder_Type *)TRACE_ENCODER_BASE)
// The sink DMA layout differs from LTraceSinkDmaType; do not use l_trace_sink_dma_read() here.
#define TRACE_SINK_DMA0          ((Penguin26TraceSinkDma_Type *)TRACE_SINK_DMA_BASE)


// ================================
//  Interrupts
// ================================
// PLIC sources. [DTS] interrupt-controller@c000000 riscv,ndev = <1>, riscv,max-priority = <1>;
// serial@10020000 interrupts = <1>. (Source 0 is reserved by the PLIC spec.)
typedef enum {
  UART0_IRQn                = 1,
} PLIC_IRQn_Type;

#define PLIC_NUM_SOURCES         1
#define PLIC_MAX_PRIORITY        1


#ifdef __cplusplus
}
#endif

#endif // __CHIP_CONFIG_H
