#ifndef __PERF_LBR_H
#define __PERF_LBR_H

#include <stdint.h>
#include <stddef.h>
#include "riscv_encoding.h"
#include "pmu.h"

#define LBR_BUFFER_SIZE 8192
#define LBR_RECORD_NUM 8

static uint64_t lbr_buffer[LBR_BUFFER_SIZE];
static size_t lbr_buffer_counter;

void lbr_init();
void lbr_fetch_records();
void lbr_dump_records();
void lbr_stop();
#endif
