#include "lbr.h"

void lbr_init() {
  asm volatile ("csrw %0, %1" :: "n"(CSR_LBR_CTRL), "i"(3));
  lbr_buffer_counter = 0;
}

void lbr_fetch_records() {
  uint64_t src0, dst0;
  asm volatile ("csrr %0, %1" : "=r"(src0) : "n"(CSR_LBR_SRC0));
  asm volatile ("csrr %0, %1" : "=r"(dst0) : "n"(CSR_LBR_DST0));
  lbr_buffer[lbr_buffer_counter + 0] = src0;
  lbr_buffer[lbr_buffer_counter + 1] = dst0;
  lbr_buffer_counter += 2;

  uint64_t src1, dst1;
  asm volatile ("csrr %0, %1" : "=r"(src1) : "n"(CSR_LBR_SRC1));
  asm volatile ("csrr %0, %1" : "=r"(dst1) : "n"(CSR_LBR_DST1));
  lbr_buffer[lbr_buffer_counter + 0] = src1;
  lbr_buffer[lbr_buffer_counter + 1] = dst1;
  lbr_buffer_counter += 2;

  uint64_t src2, dst2;
  asm volatile ("csrr %0, %1" : "=r"(src2) : "n"(CSR_LBR_SRC2));
  asm volatile ("csrr %0, %1" : "=r"(dst2) : "n"(CSR_LBR_DST2));
  lbr_buffer[lbr_buffer_counter + 0] = src2;
  lbr_buffer[lbr_buffer_counter + 1] = dst2;
  lbr_buffer_counter += 2;

  uint64_t src3, dst3;
  asm volatile ("csrr %0, %1" : "=r"(src3) : "n"(CSR_LBR_SRC3));
  asm volatile ("csrr %0, %1" : "=r"(dst3) : "n"(CSR_LBR_DST3));
  lbr_buffer[lbr_buffer_counter + 0] = src3;
  lbr_buffer[lbr_buffer_counter + 1] = dst3;
  lbr_buffer_counter += 2;

  uint64_t src4, dst4;
  asm volatile ("csrr %0, %1" : "=r"(src4) : "n"(CSR_LBR_SRC4));
  asm volatile ("csrr %0, %1" : "=r"(dst4) : "n"(CSR_LBR_DST4));
  lbr_buffer[lbr_buffer_counter + 0] = src4;
  lbr_buffer[lbr_buffer_counter + 1] = dst4;
  lbr_buffer_counter += 2;

  uint64_t src5, dst5;
  asm volatile ("csrr %0, %1" : "=r"(src5) : "n"(CSR_LBR_SRC5));
  asm volatile ("csrr %0, %1" : "=r"(dst5) : "n"(CSR_LBR_DST5));
  lbr_buffer[lbr_buffer_counter + 0] = src5;
  lbr_buffer[lbr_buffer_counter + 1] = dst5;
  lbr_buffer_counter += 2;

  uint64_t src6, dst6;
  asm volatile ("csrr %0, %1" : "=r"(src6) : "n"(CSR_LBR_SRC6));
  asm volatile ("csrr %0, %1" : "=r"(dst6) : "n"(CSR_LBR_DST6));
  lbr_buffer[lbr_buffer_counter + 0] = src6;
  lbr_buffer[lbr_buffer_counter + 1] = dst6;
  lbr_buffer_counter += 2;

  uint64_t src7, dst7;
  asm volatile ("csrr %0, %1" : "=r"(src7) : "n"(CSR_LBR_SRC7));
  asm volatile ("csrr %0, %1" : "=r"(dst7) : "n"(CSR_LBR_DST7));
  lbr_buffer[lbr_buffer_counter + 0] = src7;
  lbr_buffer[lbr_buffer_counter + 1] = dst7;
  lbr_buffer_counter += 2;
}

void lbr_dump_records() {
  lbr_stop();
  printf("LBR dump: %zu records\n", lbr_buffer_counter / 2);
  for (size_t i = 0; i < lbr_buffer_counter; i += 2) {
    printf("%lx,%lx\n", lbr_buffer[i], lbr_buffer[i + 1]);
  }
}

inline void lbr_stop() {
  asm volatile ("csrw %0, %1" :: "n"(CSR_LBR_CTRL), "i"(0));
}
