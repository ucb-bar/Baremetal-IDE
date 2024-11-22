#include <stdint.h>
#include "riscv.h"
#include "riscv_encoding.h"
#include "pmu.h"
#define NUM_ITERS 4
#define LBR_NUM 8

int main(int argc, char **argv) {
  // enable LBR
  asm volatile ("csrw %0, %1" :: "n"(CSR_LBR_CTRL), "i"(1));
  volatile int c;
  // do some dummy loops
  for (int i = 0; i < NUM_ITERS; i++) {
    c += i;
  }
  // do some different loops
  for (int i = 0; i < NUM_ITERS; i++) {
    c -= i;
  }
  // do a jump calling a dummy function
  volatile int d = dummy_function(c);
  // disable LBR
  asm volatile ("csrw %0, %1" :: "n"(CSR_LBR_CTRL), "i"(0));
  // read LBR0
  uint64_t src, dst;
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC0));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST0));
  printf("LBR[0]: src = %lx, dst = %lx\n", src, dst);
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC1));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST1));
  printf("LBR[1]: src = %lx, dst = %lx\n", src, dst); 
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC2));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST2));
  printf("LBR[2]: src = %lx, dst = %lx\n", src, dst);
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC3));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST3));
  printf("LBR[3]: src = %lx, dst = %lx\n", src, dst);
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC4));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST4));
  printf("LBR[4]: src = %lx, dst = %lx\n", src, dst);
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC5));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST5));
  printf("LBR[5]: src = %lx, dst = %lx\n", src, dst);
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC6));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST6));
  printf("LBR[6]: src = %lx, dst = %lx\n", src, dst);
  asm volatile ("csrr %0, %1" : "=r"(src) : "n"(CSR_LBR_SRC7));
  asm volatile ("csrr %0, %1" : "=r"(dst) : "n"(CSR_LBR_DST7));
  printf("LBR[7]: src = %lx, dst = %lx\n", src, dst);
  return 0;
}

__attribute__((noinline))
int dummy_function(int i) {
  int j = i + 1;
  return j;
}