/**
 * @file hal_atlas.h
 * @brief Host-side driver for the Atlas NPU tile on the penguin26 chip.
 *
 * Every offset below is taken from the atlas-npu RTL (generators/atlas-npu, SHA 0079c05):
 *   - IMEM / CSR / VMEM bases and sizes: src/main/scala/atlas/scalar/ScalarCore.scala, object
 *     AtlasMemMap (IMEM_BASE 0x2_0000, IMEM_SIZE 128 KiB, CSR_BASE 0x4_0000, CSR window 4 KiB,
 *     VMEM_BASE 0x2000_0000, VMEM_SIZE 1.5 MiB). Cross-checked against the EE290SimConfig
 *     elaboration address map ("Generated Address Map" printed by
 *     `make -C sims/vcs CONFIG=EE290SimConfig verilog` in Chipyard).
 *   - IMEM port: src/main/scala/diplomatic/top/AtlasTile.scala imemNode: 32-bit beat,
 *     TransferSizes(1, 4), i.e. word (or smaller) accesses only.
 *   - CSR register layout: src/main/scala/diplomatic/memory/CSRFile.scala (header comment and
 *     readWord(), word index = byte offset / 4):
 *       0x00 cycle counter       RW
 *       0x04 instruction counter RW
 *       0x08 status              RO  bit 0 = halted, bits [2:1] = halt_reason
 *       0x0C illegal-instr PC    RO
 *       0x10 dbg0                RW  (the test programs write a nonzero value here when done)
 *       0x14 dbg1                RW  (perf tests bank a timed-window cycle delta here)
 *       0x18 execControl         RW  bit 0: 1 = start, 0 = stop; auto-clears on a scalar halt
 *     halt_reason: 0 = none, 1 = illegal instruction, 2 = ecall, 3 = ebreak.
 *     Writing execControl=1 also clears halt_reason and the illegal PC (CSRFile.scala:150-157).
 *   - Host start/stop semantics: ScalarCore.scala:238-242 (hostStart / hostStop).
 *
 * The usage sequence mirrors the C harness emitted by generators/atlas-npu/baremetal/assembler.py
 * (emit_c_file): stop, clear dbg0/dbg1, load IMEM, verify, start, poll dbg0, stop, read CSRs.
 */
#ifndef __HAL_ATLAS_H
#define __HAL_ATLAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "metal.h"

/* CSR window, one 32-bit register per word (CSRFile.scala readWord()). */
typedef struct {
  __IO uint32_t CYCLE;        /* 0x00 */
  __IO uint32_t INSTCNT;      /* 0x04 */
  __I  uint32_t STATUS;       /* 0x08 */
  __I  uint32_t ILLEGAL_PC;   /* 0x0C */
  __IO uint32_t DBG0;         /* 0x10 */
  __IO uint32_t DBG1;         /* 0x14 */
  __IO uint32_t EXEC_CONTROL; /* 0x18 */
} ATLAS_Type;

#define ATLAS_EXEC_STOP             0U
#define ATLAS_EXEC_START            1U

#define ATLAS_STATUS_HALTED_POS     0U
#define ATLAS_STATUS_HALTED_MSK     (0x1U << ATLAS_STATUS_HALTED_POS)
#define ATLAS_STATUS_REASON_POS     1U
#define ATLAS_STATUS_REASON_MSK     (0x3U << ATLAS_STATUS_REASON_POS)

typedef enum {
  ATLAS_HALT_NONE    = 0,
  ATLAS_HALT_ILLEGAL = 1,
  ATLAS_HALT_ECALL   = 2,
  ATLAS_HALT_EBREAK  = 3,
} ATLAS_HaltReason;

/* Poll budget used by assembler.py's harness ("for (i = 0; i < 200000000U; i++)"). */
#define ATLAS_DEFAULT_POLL_LIMIT    200000000U

static inline void atlas_fence(void) { asm volatile ("fence" ::: "memory"); }

/* ---- execution control ---- */
static inline void atlas_start(ATLAS_Type *atlas) { atlas->EXEC_CONTROL = ATLAS_EXEC_START; }
static inline void atlas_stop(ATLAS_Type *atlas)  { atlas->EXEC_CONTROL = ATLAS_EXEC_STOP; }
static inline uint32_t atlas_is_running(ATLAS_Type *atlas) { return atlas->EXEC_CONTROL & 0x1U; }

/* ---- status / counters ---- */
static inline uint32_t atlas_get_cycles(ATLAS_Type *atlas)     { return atlas->CYCLE; }
static inline uint32_t atlas_get_instret(ATLAS_Type *atlas)    { return atlas->INSTCNT; }
static inline uint32_t atlas_get_status(ATLAS_Type *atlas)     { return atlas->STATUS; }
static inline uint32_t atlas_get_illegal_pc(ATLAS_Type *atlas) { return atlas->ILLEGAL_PC; }
static inline uint32_t atlas_get_dbg0(ATLAS_Type *atlas)       { return atlas->DBG0; }
static inline uint32_t atlas_get_dbg1(ATLAS_Type *atlas)       { return atlas->DBG1; }
static inline void atlas_set_dbg0(ATLAS_Type *atlas, uint32_t v) { atlas->DBG0 = v; }
static inline void atlas_set_dbg1(ATLAS_Type *atlas, uint32_t v) { atlas->DBG1 = v; }
static inline uint32_t atlas_is_halted(ATLAS_Type *atlas) {
  return (atlas->STATUS & ATLAS_STATUS_HALTED_MSK) >> ATLAS_STATUS_HALTED_POS;
}
static inline ATLAS_HaltReason atlas_get_halt_reason(ATLAS_Type *atlas) {
  return (ATLAS_HaltReason)((atlas->STATUS & ATLAS_STATUS_REASON_MSK) >> ATLAS_STATUS_REASON_POS);
}

/**
 * Stop the core and clear dbg0/dbg1 (the "Stopping Atlas core before programming IMEM" step).
 */
void atlas_reset_for_load(ATLAS_Type *atlas);

/**
 * Write a program into IMEM, one 32-bit word at a time, followed by a fence.
 * @return 0 on success, -1 if the program does not fit in IMEM.
 */
int atlas_load_program(volatile uint32_t *imem, const uint32_t *program, uint32_t n_words);

/**
 * Read IMEM back and compare against @p program.
 * @param verbose if nonzero, print one "MISMATCH word[...]" line per bad word (same text as
 *                assembler.py's harness).
 * @return number of mismatching words (0 = OK).
 */
uint32_t atlas_verify_program(volatile uint32_t *imem, const uint32_t *program, uint32_t n_words,
                              int verbose);

/**
 * Poll dbg0 until it becomes nonzero or @p max_polls reads have been made.
 * @return the last dbg0 value read (0 means it timed out).
 */
uint32_t atlas_wait_dbg0(ATLAS_Type *atlas, uint32_t max_polls);

/**
 * Poll the status register until the scalar core reports halted, or @p max_polls reads.
 * @return 1 if halted, 0 on timeout.
 */
int atlas_wait_halted(ATLAS_Type *atlas, uint32_t max_polls);

/* ---- host-side memory helpers used for test data (DRAM preload / golden checks) ---- */
static inline void atlas_mem_write32(uint64_t addr, uint32_t val) { *(volatile uint32_t *)addr = val; }
static inline uint32_t atlas_mem_read32(uint64_t addr) { return *(volatile uint32_t *)addr; }

/** Write n (address, word) pairs, then fence. */
void atlas_mem_preload(const uint64_t *addrs, const uint32_t *words, uint32_t n);

/**
 * Compare n (address, expected word) pairs.
 * @param verbose print one "DRAM MISMATCH word[...]" line per bad word (assembler.py text).
 * @return number of mismatching words.
 */
uint32_t atlas_mem_check(const uint64_t *addrs, const uint32_t *expected, uint32_t n, int verbose);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_ATLAS_H */
