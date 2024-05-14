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
  printf("EXCEPTION: instruction address misaligned\n");
  while (1) {}
}

__attribute__((weak)) void instructionAccessFaultExceptionHandler() {
  printf("EXCEPTION: instruction access fault\n");
  while (1) {}
}

__attribute__((weak)) void illegalInstructionExceptionHandler() {
  printf("EXCEPTION: illegal instruction\n");
  while (1) {}
}

__attribute__((weak)) void breakpointExceptionHandler() {
  printf("EXCEPTION: breakpoint\n");
  while (1) {}
}

__attribute__((weak)) void loadAddressMisalignedExceptionHandler() {
  printf("EXCEPTION: load address misaligned\n");
  while (1) {}
}

__attribute__((weak)) void loadAccessFaultExceptionHandler() {
  printf("EXCEPTION: load access fault\n");
  while (1) {}
}

__attribute__((weak)) void storeAMOAddressMisalignedExceptionHandler() {
  printf("EXCEPTION: store/AMO address misaligned\n");
  while (1) {}
}

__attribute__((weak)) void storeAMOAccessFaultExceptionHandler() {
  printf("EXCEPTION: store/AMO access fault\n");
  while (1) {}
}

__attribute__((weak)) void environmentCallUModeExceptionHandler() {
  // TODO: implement mode transfer
  printf("EXCEPTION: environment call from U-mode\n");
  while (1) {}
}

__attribute__((weak)) void environmentCallMModeExceptionHandler() {
  // TODO: implement mode transfer
  printf("EXCEPTION: environment call from M-mode\n");
  while (1) {}
}

__attribute__((weak)) void instructionPageFaultExceptionHandler() {
  printf("EXCEPTION: instruction page fault\n");
  while (1) {}
}

/* ==== Interrupt Callbacks ==== */
__attribute__((weak)) void userSoftwareInterruptCallback() {
  printf("user software irq\n");
  // CORE_clearIRQ(UserSoftware_IRQn);
}

__attribute__((weak)) void supervisorSoftwareInterruptCallback() {
  printf("supervisor software irq\n");
  // CORE_clearIRQ(SupervisorSoftware_IRQn);
}

__attribute__((weak)) void hypervisorSoftwareInterruptCallback() {
  printf("hypervisor software irq\n");
  // CORE_clearIRQ(HypervisorSoftware_IRQn);
}

__attribute__((weak)) void machineSoftwareInterruptCallback() {
  printf("machine software irq\n");
  // uint32_t hartid = CORE_getHartId();
  // CLINT_clearSoftwareInterrupt(hartid);
  // CORE_clearIRQ(MachineSoftware_IRQn);
}

__attribute__((weak)) void userTimerInterruptCallback() {
  printf("user timer irq\n");
  // CORE_clearIRQ(UserTimer_IRQn);
}

__attribute__((weak)) void supervisorTimerInterruptCallback() {
  printf("supervisor timer irq\n");
  // CORE_clearIRQ(SupervisorTimer_IRQn);
}

__attribute__((weak)) void hypervisorTimerInterruptCallback() {
  printf("hypervisor timer irq\n");
  // CORE_clearIRQ(HypervisorTimer_IRQn);
}

__attribute__((weak)) void machineTimerInterruptCallback() {
  printf("machine timer irq\n");
  // uint32_t hartid = CORE_getHartId();
  // CLINT_setTimerInterruptTarget(hartid, 0xFFFFFFFFFFFFFFFF);
}

__attribute__((weak)) void machineExternalInterruptCallback() {
  printf("machine external irq\n");
  // uint32_t irq_source = PLIC_claimIRQ(0);

  // { // debug message
//     char str[32];
//     sprintf(str, "machine external irq: %lu\n", irq_source);
//     UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//   }
//   switch (irq_source) {
//     case 2:                 // GPIO interrupt
//       GPIO_Callback();
//       GPIOA->HIGH_IE = 0b0;
//       break;
//   }
  // PLIC_completeIRQ(0, irq_source);
  // CORE_clearIRQ(MachineExternal_IRQn);
}

uintptr_t trap_handler(uintptr_t m_epc, uintptr_t m_cause, uintptr_t m_tval, uintptr_t regs[32]) {
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
void  __attribute__ ((weak,noreturn)) __main(void)
{
    for (;;) {
        __asm__ __volatile__ ("wfi");
    }
}

