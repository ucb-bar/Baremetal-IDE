/**
 * @file hal_core.c
 * @author -T.K.- / t_k_233@outlook.com
 * @brief 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "hal_core.h"


/* ==== Exception Handlers ==== */
__attribute__((weak)) void instructionAddressMisalignedExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void instructionAccessFaultExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void illegalInstructionExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void breakpointExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void loadAddressMisalignedExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void loadAccessFaultExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void storeAMOAddressMisalignedExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void storeAMOAccessFaultExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void environmentCallUModeExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void environmentCallMModeExceptionHandler() {
  while (1) {}
}

__attribute__((weak)) void instructionPageFaultExceptionHandler() {
  while (1) {}
}

/* ==== Interrupt Callbacks ==== */
__attribute__((weak)) void userSoftwareInterruptCallback() {}

__attribute__((weak)) void supervisorSoftwareInterruptCallback() {}

__attribute__((weak)) void hypervisorSoftwareInterruptCallback() {}

__attribute__((weak)) void machineSoftwareInterruptCallback() {}

__attribute__((weak)) void userTimerInterruptCallback() {}

__attribute__((weak)) void supervisorTimerInterruptCallback() {}

__attribute__((weak)) void hypervisorTimerInterruptCallback() {}

__attribute__((weak)) void machineTimerInterruptCallback() {}

__attribute__((weak)) void machineExternalInterruptCallback() {}

__attribute__((weak,noreturn)) uintptr_t trap_handler(uintptr_t m_epc, uintptr_t m_cause, uintptr_t m_tval, uintptr_t regs[32]) {
  switch (m_cause) {
    case 0x00000000UL:      // instruction address misaligned
      instructionAddressMisalignedExceptionHandler();
      break;
    case 0x00000001UL:      // instruction access fault
      instructionAccessFaultExceptionHandler();
      break;
    case 0x00000002UL:      // illegal instruction
      illegalInstructionExceptionHandler();
      break;
    case 0x00000003UL:      // breakpoint
      breakpointExceptionHandler();
      break;
    case 0x00000004UL:      // load address misaligned
      loadAddressMisalignedExceptionHandler();
      break;
    case 0x00000005UL:      // load access fault
      loadAccessFaultExceptionHandler();
      break;
    case 0x00000006UL:      // store/AMO address misaligned
      storeAMOAddressMisalignedExceptionHandler();
      break;
    case 0x00000007UL:      // store/AMO access fault
      storeAMOAccessFaultExceptionHandler();
      break;
    case 0x00000008UL:      // environment call from U-mode
      environmentCallUModeExceptionHandler();
      break;
    case 0x0000000BUL:      // environment call from M-mode
      environmentCallMModeExceptionHandler();
      break;
    case 0x0000000CUL:      // instruction page fault
      instructionPageFaultExceptionHandler();
      break;
    case (1UL << (RISCV_XLEN-1)) | 0x00000003UL:      // machine software interrupt
      machineSoftwareInterruptCallback();
      break;
    case (1UL << (RISCV_XLEN-1)) | 0x00000007UL:      // machine timer interrupt
      machineTimerInterruptCallback();
      break;
    case (1UL << (RISCV_XLEN-1)) | 0x0000000BUL:      // machine external interrupt
      machineExternalInterruptCallback();
      break;
    default:
      break;
  }
  return m_epc;
}


/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
__attribute__((weak,noreturn)) void __main(void) {
  while (1) {
    __asm__ __volatile__ ("wfi");
  }
}

