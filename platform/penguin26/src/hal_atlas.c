#include <stdio.h>

#include "chip_config.h"
#include "hal_atlas.h"

void atlas_reset_for_load(ATLAS_Type *atlas) {
  atlas_stop(atlas);
  atlas_set_dbg0(atlas, 0);
  atlas_set_dbg1(atlas, 0);
  atlas_fence();
}

int atlas_load_program(volatile uint32_t *imem, const uint32_t *program, uint32_t n_words) {
  if ((uint64_t)n_words * 4U > ATLAS_IMEM_SIZE) {
    return -1;
  }
  for (uint32_t i = 0; i < n_words; i++) {
    imem[i] = program[i];
  }
  atlas_fence();
  return 0;
}

uint32_t atlas_verify_program(volatile uint32_t *imem, const uint32_t *program, uint32_t n_words,
                              int verbose) {
  uint32_t fail = 0;
  for (uint32_t i = 0; i < n_words; i++) {
    uint32_t got = imem[i];
    if (got != program[i]) {
      if (verbose) {
        printf("MISMATCH word[%u]: expected 0x%016x, got 0x%08x\n",
               (unsigned)i, (unsigned)program[i], (unsigned)got);
      }
      fail++;
    }
  }
  return fail;
}

uint32_t atlas_wait_dbg0(ATLAS_Type *atlas, uint32_t max_polls) {
  uint32_t dbg0 = 0;
  for (uint32_t i = 0; i < max_polls; i++) {
    dbg0 = atlas_get_dbg0(atlas);
    if (dbg0 != 0) break;
  }
  return dbg0;
}

int atlas_wait_halted(ATLAS_Type *atlas, uint32_t max_polls) {
  for (uint32_t i = 0; i < max_polls; i++) {
    if (atlas_is_halted(atlas)) return 1;
  }
  return 0;
}

void atlas_mem_preload(const uint64_t *addrs, const uint32_t *words, uint32_t n) {
  for (uint32_t i = 0; i < n; i++) {
    atlas_mem_write32(addrs[i], words[i]);
  }
  atlas_fence();
}

uint32_t atlas_mem_check(const uint64_t *addrs, const uint32_t *expected, uint32_t n, int verbose) {
  uint32_t fail = 0;
  for (uint32_t i = 0; i < n; i++) {
    uint32_t got = atlas_mem_read32(addrs[i]);
    if (got != expected[i]) {
      if (verbose) {
        printf("  DRAM MISMATCH word[%u] @ 0x%08X%08X: expected 0x%08x, got 0x%08x\n",
               (unsigned)i,
               (unsigned)((uint64_t)addrs[i] >> 32),
               (unsigned)((uint64_t)addrs[i] & 0xFFFFFFFFU),
               (unsigned)expected[i], (unsigned)got);
      }
      fail++;
    }
  }
  return fail;
}
