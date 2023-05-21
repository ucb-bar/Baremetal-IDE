
#include <stdint.h>

#include "rv_common.h"
#include "syscall.h"


__attribute__((weak)) void HAL_MachineSoftwareInterruptCallback(uint32_t hart_id) {
//   { // debug message
//     char str[32];
//     sprintf(str, "machine software irq\n");
//     HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//   }
}

__attribute__((weak)) void HAL_GPIO_Callback() {}

void InstructionAddressMisalign_Exception_Handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "instruction address misalign exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}
void InstructionAccessFault_Exception_Handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "instruction access fault exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}
void IllegalInstruction_Exception_Handler() {
  // while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "illegal instruction exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}
void Breakpoint_Exception_Handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "breakpoint exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
  // }
}
void LoadAddressMisalign_Exception_handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "load address misaligned exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}
void LoadAccessFault_Exception_handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "load access fault exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}
void StoreAMOAddressMisalign_Exception_handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "store/AMO address misaligned exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}
void StoreAMOAccessFault_Exception_handler() {
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "store/AMO access fault exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
  // }
}
void EnvironmentCallUMode_Exception_handler() {
//   // TODO: implement mode transfer
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "environment call from U-mode exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
  // }
}
void EnvironmentCallMMode_Exception_handler() {
//   // TODO: implement mode transfer
//   while (1) {
//     { // debug message
//       char str[64];
//       sprintf(str, "environment call from M-mode exception\n");
//       HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//     }
//   }
}

void UserSoftware_IRQn_Handler() {}
void SupervisorSoftware_IRQn_Handler() {}
void HypervisorSoftware_IRQn_Handler() {}
void MachineSoftware_IRQn_Handler() {
  // HAL_MachineSoftwareInterruptCallback(0);
  // CLINT->MSIP0 = 0;
  // CLINT->MSIP1 = 0;
  // CLINT->MSIP2 = 0;
  // CLINT->MSIP3 = 0;
  // CLINT->MSIP4 = 0;
  // HAL_CORE_clearIRQ(MachineSoftware_IRQn);
}

void UserTimer_IRQn_Handler() {}
void SupervisorTimer_IRQn_Handler() {}
void HypervisorTimer_IRQn_Handler() {}
void MachineTimer_IRQn_Handler() {
//   { // debug message
//     char str[32];
//     sprintf(str, "machine timer irq\n");
//     HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//   }
  // HAL_CLINT_setTimerInterrupt(0xFFFFFFFFFFFFFFFF);
  // HAL_CORE_clearIRQ(MachineTimer_IRQn);
}

void UserExternal_IRQn_Handler() {}
void SupervisorExternal_IRQn_Handler() {}
void HypervisorExternal_IRQn_Handler() {}
void MachineExternal_IRQn_Handler() {
  // uint32_t irq_source = HAL_PLIC_claimIRQ(0);

  // { // debug message
//     char str[32];
//     sprintf(str, "machine external irq: %lu\n", irq_source);
//     HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
//   }
//   switch (irq_source) {
//     case 2:                 // GPIO interrupt
//       HAL_GPIO_Callback();
//       GPIOA->HIGH_IE = 0b0;
//       break;
//   }
  // HAL_PLIC_completeIRQ(0, irq_source);
  
  // HAL_CORE_clearIRQ(MachineExternal_IRQn);
}

void system_init(uint32_t hartid) {

  // TODO: these should really go into main(), but putting here temporarily for ease of testing
  // HAL_init();
  
  return;
}

void trap_handler(uintptr_t m_epc, uintptr_t m_cause, uintptr_t m_tval, uintptr_t regs[32]) {

  // /* Extract low-order bits of exception code as positive int */
  // int code = cause & ((1UL << ((sizeof(int)<<3)-1)) - 1);
  // /* Encode interrupt as negative value */
  // code = ((intptr_t)cause < 0) ? -code : code;
  // _exit(code);
  // __builtin_unreachable();

  
  switch (m_cause) {
    case 0x00000000:      // instruction address misaligned
      InstructionAddressMisalign_Exception_Handler();
      break;
    case 0x00000001:      // instruction access fault
      InstructionAccessFault_Exception_Handler();
      break;
    case 0x00000002:      // illegal instruction
      IllegalInstruction_Exception_Handler();
      break;
    case 0x00000003:      // breakpoint
      Breakpoint_Exception_Handler();
      break;
    case 0x00000004:      // load address misaligned
      LoadAddressMisalign_Exception_handler();
      break;
    case 0x00000005:      // load access fault
      LoadAccessFault_Exception_handler();
      break;
    case 0x00000006:      // store/AMO address misaligned
      StoreAMOAddressMisalign_Exception_handler();
      break;
    case 0x00000007:      // store/AMO access fault
      StoreAMOAccessFault_Exception_handler();
      break;
    case 0x00000008:      // environment call from U-mode
      EnvironmentCallUMode_Exception_handler();
      break;
    case 0x00000011:      // environment call from M-mode
      EnvironmentCallMMode_Exception_handler();
      break;
    case 0x80000003:      // machine software interrupt
      MachineSoftware_IRQn_Handler();
      break;
    case 0x80000007:      // machine timer interrupt
      MachineTimer_IRQn_Handler();
      break;
    case 0x8000000B:      // machine external interrupt
      MachineExternal_IRQn_Handler();
      break;
    default:
      break;
      // { // debug message
      //   char str[32];
      //   sprintf(str, "mcause %lu\n", m_cause);
      //   HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
      // }
  }
}
