#ifndef __TEST_DMA
#define __TEST_DMA

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "riscv.h"

#define DMA_ADDR1 0x90000000L
#define DMA_ADDR2 0x90100000L
#define DMA_ADDR3 0x90200000L
#define DMA_ADDR4 0x90300000L
#define DMA_ADDR5 0x90400000L
#define DMA_ADDR6 0x90500000L


int checkval(int i, long unsigned int  poll, long unsigned int ref, long unsigned int addr);

int test_basic();
int test_combined();
int test_stride();
int test_width();

#ifdef __cplusplus
}
#endif

#endif /* __TEST_DMA */
