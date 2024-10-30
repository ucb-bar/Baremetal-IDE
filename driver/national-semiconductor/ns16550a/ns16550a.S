/** 
 * original author: safinsingh
 * available at: https://github.com/safinsingh/ns16550a.git
 */
 
.global uart_init
.global uart_putc
.global uart_getc

#include "uart_const.h"

uart_init:
.cfi_startproc
  li  t0, UART_ADDRESS

  # 0x3 -> 8 bit word length
  li  t1, 0x3
  sb  t1, LINE_CONTROL_REGISTER(t0)

  # 0x1 -> enable FIFOs
  li  t1, 0x1
  sb  t1, LINE_CONTROL_REGISTER(t0)

  # 0x1 -> enable reciever interrupts
  sb  t1, INTERRUPT_ENABLE_REGISTER(t0)

  ret
.cfi_endproc

uart_getc:
.cfi_startproc
  li      t0, UART_ADDRESS

  lbu     t1, LINE_STATUS_REGISTER(t0)
  andi    t1, t1, LINE_STATUS_DATA_READY

  # jump to _uart_read if UART is ready to read from
  bnez    t1, _uart_read

  # otherwise, return 0
  mv      a0, zero
  j       _uart_get_end

_uart_read:
  # load character at UART address into a0
  lbu     a0, (t0)
  j       _uart_get_end

_uart_get_end:
  ret
.cfi_endproc

uart_putc:
.cfi_startproc
  li  t0, UART_ADDRESS

  # store character at UART address in return register
  sb  a0, (t0)
  ret
.cfi_endproc