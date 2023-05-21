
#ifndef _CHIPYARD_ASM_H
#define _CHIPYARD_ASM_H

#if __riscv_xlen == 64
  #define LOAD ld
  #define STORE sd
  #define REGBYTES 8
#else
  #define LOAD lw
  #define STORE sw
  #define REGBYTES 4
#endif


#endif /* _CHIPYARD_ASM_H */
