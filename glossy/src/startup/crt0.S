#include "riscv.h"
#include "riscv_encoding.h"


.align 4
.section .text.init
.global _start
.type _start, @function
_start:
  .cfi_startproc

  /* initialize registers */
  li x1, 0
  li x2, 0
  li x3, 0
  li x4, 0
  li x5, 0
  li x6, 0
  li x7, 0
  li x8, 0
  li x9, 0
  li x10, 0
  li x11, 0
  li x12, 0
  li x13, 0
  li x14, 0
  li x15, 0
  li x16, 0
  li x17, 0
  li x18, 0
  li x19, 0
  li x20, 0
  li x21, 0
  li x22, 0
  li x23, 0
  li x24, 0
  li x25, 0
  li x26, 0
  li x27, 0
  li x28, 0
  li x29, 0
  li x30, 0
  li x31, 0

  li t0, (MSTATUS_FS | MSTATUS_XS | MSTATUS_VS)
  csrs mstatus, t0

#ifdef __riscv_flen
  /* Initialize FPU if present */
  la t0, fpu_init_exit
  csrw mtvec, t0

  fscsr zero
  fmv.s.x f0,  zero
  fmv.s.x f1,  zero
  fmv.s.x f2,  zero
  fmv.s.x f3,  zero
  fmv.s.x f4,  zero
  fmv.s.x f5,  zero
  fmv.s.x f6,  zero
  fmv.s.x f7,  zero
  fmv.s.x f8,  zero
  fmv.s.x f9,  zero
  fmv.s.x f10, zero
  fmv.s.x f11, zero
  fmv.s.x f12, zero
  fmv.s.x f13, zero
  fmv.s.x f14, zero
  fmv.s.x f15, zero
  fmv.s.x f16, zero
  fmv.s.x f17, zero
  fmv.s.x f18, zero
  fmv.s.x f19, zero
  fmv.s.x f20, zero
  fmv.s.x f21, zero
  fmv.s.x f22, zero
  fmv.s.x f23, zero
  fmv.s.x f24, zero
  fmv.s.x f25, zero
  fmv.s.x f26, zero
  fmv.s.x f27, zero
  fmv.s.x f28, zero
  fmv.s.x f29, zero
  fmv.s.x f30, zero
  fmv.s.x f31, zero
fpu_init_exit:
#endif

  /* Initialize global pointer; disable relaxation to avoid relaxing
      the address calculation to "addi gp, gp, 0" */
  .option push
  .option norelax
  la gp, __global_pointer$
  .option pop

  /* Initialize thread pointer */
  csrr s0, mhartid
  la t0, __stack_shift
  la tp, __stack_start
  sll t0, s0, t0
  add tp, tp, t0

  /* Initialize stack pointer */
  la t0, __stack_size
  add sp, tp, t0

  /* Initialize trap vector */
  la t0, trap_vector
  csrw mtvec, t0

tls_init_entry:
  /* Initialize TLS */
  mv t0, tp
  la t1, __tdata_start
  la t2, __tdata_end

  # see if:
  # - tdata section is already at correct location?
  # - tdata section is of size 0?
  beq t0, t1, tls_init_exit
  bge t1, t2, tls_init_exit

tls_init_loop:
  LREG  t3, 0(t0)
  SREG  t3, 0(tp)
  addi  t0, t0, REGBYTES
  addi  t1, t1, REGBYTES
  blt   t1, t2, tls_init_loop

tls_init_exit:

  /* Skip global initialization if not the designated boot hart */
  la t0, __boot_hart
  bne s0, t0, _start_secondary

data_init_entry:
  /* Copy data from non-volatile memory to runtime location */
  la t0, __data_load_start
  la t1, __data_start
  la t2, __data_end

  # see if:
  # - data section is already at correct location?
  # - data section is of size 0?
  beq t0, t1, data_init_exit
  bge t1, t2, data_init_exit

data_init_loop:
  LREG  t3, 0(t0)
  SREG  t3, 0(t1)
  addi  t0, t0, REGBYTES
  addi  t1, t1, REGBYTES
  blt   t1, t2, data_init_loop

data_init_exit:

bss_init_entry:
  /* Zero BSS segment */
  la t0, __bss_start
  la t1, __bss_end
  bgeu t0, t1, bss_init_exit

bss_init_loop:
  SREG zero, (t0)
  addi t0, t0, REGBYTES
  bltu t0, t1, bss_init_loop

bss_init_exit:

  /* Call global constructors */
  la a0, __libc_fini_array
  call atexit
  call __libc_init_array

_start_primary:
  /* Call main function */
  li a0, 1            /* argc = 1 */
  la a1, argv         /* argv = { "", NULL } */
  LREG a2, environ    /* envp */
  
  call main
  tail exit
  j infinite_loop


_start_secondary:
  /* Call __main function */
  li a0, 1            /* argc = 1 */
  la a1, argv         /* argv = { "", NULL } */
  LREG a2, environ    /* envp */
  
  call __main
  tail exit
  j infinite_loop


infinite_loop:
  j infinite_loop

  .cfi_endproc


.section .rodata
.align 3
argv:
  .dc.a name
  .dc.a 0
name:
  .asciz "chipyard"
