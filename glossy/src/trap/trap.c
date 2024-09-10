/**
 * @file trap.c
 */

#include <stdint.h>
#include "riscv.h"


/* ==== Exception Handlers ==== */
__attribute__((weak)) void instruction_address_misaligned_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void instruction_access_fault_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void illegal_instruction_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void breakpoint_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void load_address_misaligned_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void load_access_fault_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void store_amo_address_misaligned_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void store_amo_access_fault_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void environment_call_u_mode_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void environment_call_m_mode_exception_handler() {
  while (1) {}
}

__attribute__((weak)) void instruction_page_fault_exception_handler() {
  while (1) {}
}

/* ==== Interrupt Callbacks ==== */
__attribute__((weak)) void user_software_interrupt_callback() {}

__attribute__((weak)) void supervisor_software_interrupt_callback() {}

__attribute__((weak)) void hypervisor_software_interrupt_callback() {}

__attribute__((weak)) void machine_software_interrupt_callback() {}

__attribute__((weak)) void user_timer_interrupt_callback() {}

__attribute__((weak)) void supervisor_timer_interrupt_callback() {}

__attribute__((weak)) void hypervisor_timer_interrupt_callback() {}

__attribute__((weak)) void machine_timer_interrupt_callback() {}

__attribute__((weak)) void machine_external_interrupt_callback() {}

__attribute__((weak)) uintptr_t trap_handler(uintptr_t m_epc, uintptr_t m_cause, uintptr_t m_tval, uintptr_t regs[32]) {
  // TODO: merge this to trap.S
  switch (m_cause) {
    case 0x00000000UL:      // instruction address misaligned
      instruction_address_misaligned_exception_handler();
      break;
    case 0x00000001UL:      // instruction access fault
      instruction_access_fault_exception_handler();
      break;
    case 0x00000002UL:      // illegal instruction
      illegal_instruction_exception_handler();
      break;
    case 0x00000003UL:      // breakpoint
      breakpoint_exception_handler();
      break;
    case 0x00000004UL:      // load address misaligned
      load_address_misaligned_exception_handler();
      break;
    case 0x00000005UL:      // load access fault
      load_access_fault_exception_handler();
      break;
    case 0x00000006UL:      // store/AMO address misaligned
      store_amo_address_misaligned_exception_handler();
      break;
    case 0x00000007UL:      // store/AMO access fault
      store_amo_access_fault_exception_handler();
      break;
    case 0x00000008UL:      // environment call from U-mode
      environment_call_u_mode_exception_handler();
      break;
    case 0x0000000BUL:      // environment call from M-mode
      environment_call_m_mode_exception_handler();
      break;
    case 0x0000000CUL:      // instruction page fault
      instruction_page_fault_exception_handler();
      break;
    case (1UL << (RISCV_XLEN-1)) | 0x00000003UL:      // machine software interrupt
      machine_software_interrupt_callback();
      break;
    case (1UL << (RISCV_XLEN-1)) | 0x00000007UL:      // machine timer interrupt
      machine_timer_interrupt_callback();
      break;
    case (1UL << (RISCV_XLEN-1)) | 0x0000000BUL:      // machine external interrupt
      machine_external_interrupt_callback();
      break;
    default:
      break;
  }
  return m_epc;
}
