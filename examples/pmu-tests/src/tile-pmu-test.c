#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "tile_pmu_platform.h"

static unsigned checks;
#define CHECK(condition) do { \
  ++checks; \
  if (!(condition)) { \
    printf("FAIL line %u: %s\n", (unsigned)__LINE__, #condition); \
    exit(1); \
  } \
} while (0)

struct snapshot { uint64_t values[TILE_PMU_COUNTER_COUNT]; };
static volatile uint64_t memory[4096];

static void read_snapshot(struct snapshot *snapshot) {
#define READ_EVENT(symbol, id, key, unit) \
  snapshot->values[id] = tile_pmu_read_counter(TILE_PMU_BASE, id);
  TILE_PMU_EVENTS(READ_EVENT)
#undef READ_EVENT
}

static void capture(struct snapshot *snapshot) {
  tile_pmu_command(TILE_PMU_BASE, PMU_COMMAND_SNAPSHOT);
  read_snapshot(snapshot);
}

static void check_equal(const struct snapshot *a, const struct snapshot *b) {
  for (unsigned id = 0; id < TILE_PMU_COUNTER_COUNT; ++id)
    CHECK(a->values[id] == b->values[id]);
}

static void check_zero(const struct snapshot *snapshot) {
  for (unsigned id = 0; id < TILE_PMU_COUNTER_COUNT; ++id)
    CHECK(snapshot->values[id] == 0);
}

/* Exactly four instructions per iteration. CSR observations bound the work;
 * the MMIO counting window also includes control/readout instructions. */
static __attribute__((noinline)) uint64_t instruction_work(unsigned iterations) {
  uint64_t begin, end, value = 1;
  __asm__ volatile (
    ".option push\n.option norvc\n"
    "csrr %[begin], minstret\n"
    "1: addi %[value], %[value], 1\n"
    "xori %[value], %[value], 7\n"
    "addi %[iterations], %[iterations], -1\n"
    "bnez %[iterations], 1b\n"
    "csrr %[end], minstret\n"
    ".option pop\n"
    : [begin] "=&r" (begin), [end] "=r" (end),
      [value] "+r" (value), [iterations] "+r" (iterations)
    : : "memory");
  return end - begin;
}

static void memory_work(void) {
  for (unsigned i = 0; i < 4096; i += 8)
    memory[i] = i + 1;
  for (unsigned i = 0; i < 4096; i += 8)
    memory[i] ^= 0x1234;
}

int main(void) {
  struct snapshot zero, first, stable, later, stopped, cleared;
  const uintptr_t base = TILE_PMU_BASE;
  uint32_t info_lo = tile_pmu_read32(base, PMU_MMIO_INFO);
  uint32_t info_hi = tile_pmu_read32(base, PMU_MMIO_INFO + 4);
  printf("TilePMU source=%s base=0x%" PRIxPTR " counters=%u width=%u\n",
         TILE_PMU_SOURCE, base, PMU_INFO_COUNT(info_lo), PMU_INFO_WIDTH(info_hi));
  CHECK(PMU_INFO_VERSION(info_lo) == PMU_MMIO_VERSION);
  CHECK(PMU_INFO_COUNT(info_lo) == TILE_PMU_COUNTER_COUNT);
  CHECK(PMU_INFO_WIDTH(info_hi) == TILE_PMU_COUNTER_BITS);
  CHECK(tile_pmu_read32(base, PMU_MMIO_STATUS) == 0);
  CHECK(tile_pmu_read32(base, PMU_MMIO_CONTROL) == 0);
  read_snapshot(&zero);
  check_zero(&zero);

  tile_pmu_command(base, PMU_COMMAND_CLEAR | PMU_COMMAND_SNAPSHOT);
  CHECK(tile_pmu_read32(base, PMU_MMIO_STATUS) == PMU_STATUS_SNAPSHOT_VALID);
  read_snapshot(&zero);
  check_zero(&zero);
  instruction_work(32);
  capture(&stable);
  check_zero(&stable);

  tile_pmu_enable(base, 1);
  uint64_t retired = instruction_work(1024);
  tile_pmu_enable(base, 0);
  capture(&first);
  CHECK(retired == 4 * 1024 + 1);
  CHECK(first.values[TILE_PMU_EVENT_ROB_INSTRUCTIONS_RETIRED] >= retired);
  /* Keep a useful upper bound: this window contains only the fixed loop and
   * a few dozen control instructions, not readout/printf. */
  CHECK(first.values[TILE_PMU_EVENT_ROB_INSTRUCTIONS_RETIRED] < retired + 128);
  CHECK(first.values[TILE_PMU_EVENT_TILE_CYCLES] > 0);
  printf("Fixed loop: CSR retired=%" PRIu64 " PMU retired=%" PRIu64 "\n",
         retired, first.values[TILE_PMU_EVENT_ROB_INSTRUCTIONS_RETIRED]);

  tile_pmu_command(base, PMU_COMMAND_CLEAR);
  tile_pmu_enable(base, 1);
  CHECK(tile_pmu_read32(base, PMU_MMIO_STATUS) ==
        (PMU_STATUS_ENABLED | PMU_STATUS_SNAPSHOT_VALID));
  memory_work();
  instruction_work(512);
  capture(&first);
  instruction_work(512);
  read_snapshot(&stable);
  check_equal(&first, &stable);  /* Running bank cannot tear captured values. */
  capture(&later);
  CHECK(later.values[TILE_PMU_EVENT_TILE_CYCLES] > first.values[TILE_PMU_EVENT_TILE_CYCLES]);
  CHECK(later.values[TILE_PMU_EVENT_ROB_INSTRUCTIONS_RETIRED] >=
        first.values[TILE_PMU_EVENT_ROB_INSTRUCTIONS_RETIRED] + 4 * 512);
  CHECK(later.values[TILE_PMU_EVENT_LSU_DCACHE_LINE_MISSES] > 0);

  tile_pmu_enable(base, 0);
  CHECK(tile_pmu_read32(base, PMU_MMIO_STATUS) == PMU_STATUS_SNAPSHOT_VALID);
  capture(&stopped);
  memory_work();
  instruction_work(64);
  capture(&stable);
  check_equal(&stopped, &stable);

#if __riscv_xlen == 64
  for (unsigned id = 0; id < TILE_PMU_COUNTER_COUNT; ++id) {
    tile_pmu_fence();
    uint64_t value = *(volatile const uint64_t *)(base + PMU_MMIO_COUNTER(id));
    tile_pmu_fence();
    CHECK(value == stopped.values[id]);
  }
#endif

  /* Capture+clear returns the pre-clear count, then the next snapshot is zero. */
  tile_pmu_command(base, PMU_COMMAND_SNAPSHOT | PMU_COMMAND_CLEAR);
  read_snapshot(&stable);
  check_equal(&stopped, &stable);
  capture(&cleared);
  check_zero(&cleared);
  CHECK(tile_pmu_read32(base, PMU_MMIO_STATUS) == PMU_STATUS_SNAPSHOT_VALID);

  /* A plain clear must leave the previous snapshot intact, including validity. */
  tile_pmu_enable(base, 1);
  instruction_work(64);
  tile_pmu_enable(base, 0);
  capture(&later);
  CHECK(later.values[TILE_PMU_EVENT_ROB_INSTRUCTIONS_RETIRED] >= 4 * 64);
  tile_pmu_command(base, PMU_COMMAND_CLEAR);
  read_snapshot(&stable);
  check_equal(&later, &stable);
  capture(&cleared);
  check_zero(&cleared);

  puts("Workload snapshot (manifest keys; speculative events may be zero):");
#define PRINT_EVENT(symbol, id, key, unit) \
  printf("%s=%" PRIu64 " %s\n", key, stopped.values[id], unit);
  TILE_PMU_EVENTS(PRINT_EVENT)
#undef PRINT_EVENT
  printf("PASS TilePMU software test (%u checks)\n", checks);
  return 0;
}
