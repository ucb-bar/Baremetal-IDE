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
 */
 
#include <rv/arch.h>
#include <rv/encoding.h>


/* Signal barrier release */
.macro BARRIER_PASS flag
  li t0, -1
  fence w, w
  sw t0, \flag, t1
.endm

/* Wait at barrier */
.altmacro
.macro BARRIER_WAIT flag
  LOCAL L1, L2
L1:
  auipc t1, %pcrel_hi(\flag)
L2:
  lw t0, %pcrel_lo(L1)(t1)
  beqz t0, L2
  fence r, r
  .endm


.align 4
.section .text.trap_vector
.globl trap_vector
trap_vector:
  csrw mscratch, sp
  addi sp, sp, -(32*REGBYTES)

  /* Save trap frame */
  SREG x1, 1*REGBYTES(sp)
  csrr x1, mscratch
  SREG x1, 2*REGBYTES(sp)
  SREG x3, 3*REGBYTES(sp)
  SREG x4, 4*REGBYTES(sp)
  SREG x5, 5*REGBYTES(sp)
  SREG x6, 6*REGBYTES(sp)
  SREG x7, 7*REGBYTES(sp)
  SREG x8, 8*REGBYTES(sp)
  SREG x9, 9*REGBYTES(sp)
  SREG x10, 10*REGBYTES(sp)
  SREG x11, 11*REGBYTES(sp)
  SREG x12, 12*REGBYTES(sp)
  SREG x13, 13*REGBYTES(sp)
  SREG x14, 14*REGBYTES(sp)
  SREG x15, 15*REGBYTES(sp)
  SREG x16, 16*REGBYTES(sp)
  SREG x17, 17*REGBYTES(sp)
  SREG x18, 18*REGBYTES(sp)
  SREG x19, 19*REGBYTES(sp)
  SREG x20, 20*REGBYTES(sp)
  SREG x21, 21*REGBYTES(sp)
  SREG x22, 22*REGBYTES(sp)
  SREG x23, 23*REGBYTES(sp)
  SREG x24, 24*REGBYTES(sp)
  SREG x25, 25*REGBYTES(sp)
  SREG x26, 26*REGBYTES(sp)
  SREG x27, 27*REGBYTES(sp)
  SREG x28, 28*REGBYTES(sp)
  SREG x29, 29*REGBYTES(sp)
  SREG x30, 30*REGBYTES(sp)
  SREG x31, 31*REGBYTES(sp)

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

  LREG x1, 1*REGBYTES(sp)
  LREG x3, 3*REGBYTES(sp)
  LREG x4, 4*REGBYTES(sp)
  LREG x5, 5*REGBYTES(sp)
  LREG x6, 6*REGBYTES(sp)
  LREG x7, 7*REGBYTES(sp)
  LREG x8, 8*REGBYTES(sp)
  LREG x9, 9*REGBYTES(sp)
  LREG x10, 10*REGBYTES(sp)
  LREG x11, 11*REGBYTES(sp)
  LREG x12, 12*REGBYTES(sp)
  LREG x13, 13*REGBYTES(sp)
  LREG x14, 14*REGBYTES(sp)
  LREG x15, 15*REGBYTES(sp)
  LREG x16, 16*REGBYTES(sp)
  LREG x17, 17*REGBYTES(sp)
  LREG x18, 18*REGBYTES(sp)
  LREG x19, 19*REGBYTES(sp)
  LREG x20, 20*REGBYTES(sp)
  LREG x21, 21*REGBYTES(sp)
  LREG x22, 22*REGBYTES(sp)
  LREG x23, 23*REGBYTES(sp)
  LREG x24, 24*REGBYTES(sp)
  LREG x25, 25*REGBYTES(sp)
  LREG x26, 26*REGBYTES(sp)
  LREG x27, 27*REGBYTES(sp)
  LREG x28, 28*REGBYTES(sp)
  LREG x29, 29*REGBYTES(sp)
  LREG x30, 30*REGBYTES(sp)
  LREG x31, 31*REGBYTES(sp)
  /* Restore sp last */
  LREG x2, 2*REGBYTES(sp)

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
  lui t0, %hi(__stack_shift)
  addi t0, t0, %lo(__stack_shift)
  la tp, __stack_start
  sll t0, s0, t0
  add tp, tp, t0

  /* Initialize stack pointer */
  lui t0, %hi(__stack_size)
  addi t0, t0, %lo(__stack_size)
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
  bne s0, t0, boot_secondary

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

boot_primary:
  # /* Release signal barrier */
  # li t0, -1
  # fence w, w
  # sw t0, __boot_sync, t1

  /* Call main function */
  li a0, 1            /* argc = 1 */
  la a1, argv         /* argv = { "", NULL } */
  LREG a2, environ    /* envp */
  
  call main
  tail exit
  j infinite_loop

boot_secondary:  
  # /* Wait at barrier */
  # barrier_wait_1:
  #   auipc t1, %pcrel_hi(__boot_sync)
  # barrier_wait_2:
  #   lw t0, %pcrel_lo(barrier_wait_1)(t1)
  #   beqz t0, barrier_wait_2
  #   fence r, r

  /* Call __main function */
  li a0, 1            /* argc = 1 */
  la a1, argv         /* argv = { "", NULL } */
  LREG a2, environ    /* envp */
  
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

.bss
__boot_sync:
  .word 0

/* Define _init() and _fini() for compatibility */
.global _init
.type _init, @function
.global _fini
.type _fini, @function
_init:
_fini:
  ret

.size _init, .-_init
.size _fini, .-_fini
