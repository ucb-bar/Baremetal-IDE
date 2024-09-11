/**
 * @file startup.S
 * @brief RISC-V Architecture Definitions and Operations
 *
 * This file contains assembly code specific to the RISC-V architecture.
 * It includes trap vectors, register initialization, stack setup, BSS segment initialization,
 * and the boot manager responsible for calling initialization routines and the main function.
 *
 * The trap_vector section sets up a trap frame and invokes the trap_handler.
 * The _start section initializes registers, initializes the FPU if present, initializes global and thread pointers,
 * sets up the stack pointer, initializes the trap vector, initializes TLS, zeros the BSS segment,
 * and calls global constructors before entering the boot_manager or boot_other section.
 *
 * The boot_manager section is executed by the designated boot hart and calls system_init,
 * initializes command line arguments (argc, argv, envp), and calls the main function.
 *
 * The boot_other section is executed by non-boot harts and calls the __main function before exiting.
 *
 * @note This file contains assembly code specific to the RISC-V architecture and should be included
 *       as part of the startup or initialization code.
 *
 * @author -T.K.-
 * @date 2023-05-20
 */
 
#include "rv_arch.h"
#include "rv_encoding.h"

.align 4
.section .text.trap_vector
.globl trap_vector
trap_vector:
  csrw mscratch, sp
  addi sp, sp, -(32*REGBYTES)

  /* Save trap frame */
  STORE x1, 1*REGBYTES(sp)
  csrr x1, mscratch
  STORE x1, 2*REGBYTES(sp)
  STORE x3, 3*REGBYTES(sp)
  STORE x4, 4*REGBYTES(sp)
  STORE x5, 5*REGBYTES(sp)
  STORE x6, 6*REGBYTES(sp)
  STORE x7, 7*REGBYTES(sp)
  STORE x8, 8*REGBYTES(sp)
  STORE x9, 9*REGBYTES(sp)
  STORE x10, 10*REGBYTES(sp)
  STORE x11, 11*REGBYTES(sp)
  STORE x12, 12*REGBYTES(sp)
  STORE x13, 13*REGBYTES(sp)
  STORE x14, 14*REGBYTES(sp)
  STORE x15, 15*REGBYTES(sp)
  STORE x16, 16*REGBYTES(sp)
  STORE x17, 17*REGBYTES(sp)
  STORE x18, 18*REGBYTES(sp)
  STORE x19, 19*REGBYTES(sp)
  STORE x20, 20*REGBYTES(sp)
  STORE x21, 21*REGBYTES(sp)
  STORE x22, 22*REGBYTES(sp)
  STORE x23, 23*REGBYTES(sp)
  STORE x24, 24*REGBYTES(sp)
  STORE x25, 25*REGBYTES(sp)
  STORE x26, 26*REGBYTES(sp)
  STORE x27, 27*REGBYTES(sp)
  STORE x28, 28*REGBYTES(sp)
  STORE x29, 29*REGBYTES(sp)
  STORE x30, 30*REGBYTES(sp)
  STORE x31, 31*REGBYTES(sp)

  /* Invoke higher-level trap handler */
  csrr a0, mepc
  csrr a1, mcause
  csrr a2, mtval
  mv a3, sp
  call trap_handler
  csrw mepc, a0

  /* Remain in M-mode after return */
  li t0, MSTATUS_MPP
  csrs mstatus, t0

  LOAD x1, 1*REGBYTES(sp)
  LOAD x3, 3*REGBYTES(sp)
  LOAD x4, 4*REGBYTES(sp)
  LOAD x5, 5*REGBYTES(sp)
  LOAD x6, 6*REGBYTES(sp)
  LOAD x7, 7*REGBYTES(sp)
  LOAD x8, 8*REGBYTES(sp)
  LOAD x9, 9*REGBYTES(sp)
  LOAD x10, 10*REGBYTES(sp)
  LOAD x11, 11*REGBYTES(sp)
  LOAD x12, 12*REGBYTES(sp)
  LOAD x13, 13*REGBYTES(sp)
  LOAD x14, 14*REGBYTES(sp)
  LOAD x15, 15*REGBYTES(sp)
  LOAD x16, 16*REGBYTES(sp)
  LOAD x17, 17*REGBYTES(sp)
  LOAD x18, 18*REGBYTES(sp)
  LOAD x19, 19*REGBYTES(sp)
  LOAD x20, 20*REGBYTES(sp)
  LOAD x21, 21*REGBYTES(sp)
  LOAD x22, 22*REGBYTES(sp)
  LOAD x23, 23*REGBYTES(sp)
  LOAD x24, 24*REGBYTES(sp)
  LOAD x25, 25*REGBYTES(sp)
  LOAD x26, 26*REGBYTES(sp)
  LOAD x27, 27*REGBYTES(sp)
  LOAD x28, 28*REGBYTES(sp)
  LOAD x29, 29*REGBYTES(sp)
  LOAD x30, 30*REGBYTES(sp)
  LOAD x31, 31*REGBYTES(sp)
  /* Restore sp last */
  LOAD x2, 2*REGBYTES(sp)

  mret

.align 4
.section .text.init
.global _start
.type _start, @function
_start:
  .cfi_startproc

  # initialize registers
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

  li t0, (MSTATUS_FS | MSTATUS_XS)
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

  /* Initialize TLS */
  call __init_tls

  /* Skip global initialization if not the designated boot hart */
  la t0, __boot_hart
  bne s0, t0, boot_other

data_init_entry:
  la t0, __data_load_start
  la t1, __data_start
  la t2, __data_end

  # see if:
  # - data section is already at correct location?
  # - data section is of size 0?
  beq t0, t1, data_init_exit
  bge t1, t2, data_init_exit

data_init_loop:
  LOAD   t3, 0(t0)
  STORE t3, 0(t1)
  addi t0, t0, REGBYTES
  addi t1, t1, REGBYTES
  blt  t1, t2, data_init_loop
data_init_exit:

bss_init_entry:
  /* Zero BSS segment */
  la t0, __bss_start
  la t1, __bss_end
  bgeu t0, t1, bss_init_exit

bss_init_loop:
  STORE zero, (t0)
  addi t0, t0, REGBYTES
  bltu t0, t1, bss_init_loop

bss_init_exit:

  /* Call global constructors */
  la a0, __libc_fini_array
  call atexit
  call __libc_init_array

boot_manager:
  li a0, 1            /* argc = 1 */
  la a1, argv         /* argv = { "", NULL } */
  LOAD a2, environ    /* envp */
  
  /* Call main function */
  call main
  tail exit
  j infinite_loop


boot_other:  
  /* Call __main function */
  call __main
  tail exit
  j infinite_loop

  .cfi_endproc

infinite_loop:
  j infinite_loop


.section .rodata
.align 3
argv:
  .dc.a name
  .dc.a 0
name:
  .asciz "chipyard"

/* Define _init() and _fini() for compatibility */
.global _init
.type _init, @function
.global _fini
.type _fini, @function
_init:
_fini:
  ret