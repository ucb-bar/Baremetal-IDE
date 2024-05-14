#include <stdio.h>
#include <stdint.h>


#include <stdint.h>
#include <stddef.h>

#include "htif.h"
#include "syscall.h"

/* ================ Memory register attributes ================ */
#ifdef __cplusplus
  #define   __I     volatile             /** Defines "read only" permissions */
#else
  #define   __I     volatile const       /** Defines "read only" permissions */
#endif
#define     __O     volatile             /** Defines "write only" permissions */
#define     __IO    volatile             /** Defines "read / write" permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /** Defines "read only" structure member permissions */
#define     __OM     volatile            /** Defines "write only" structure member permissions */
#define     __IOM    volatile            /** Defines "read / write" structure member permissions */


#define UART_BASE 10000000



/* Peripheral Struct Definition */
typedef struct {
  __IO uint8_t  TXDATA;                         /** Transmit data register */
  __I  uint32_t RXDATA;                         /** Receive data register */
  __IO uint32_t TXCTRL;                         /** Transmit control register */
  __IO uint32_t RXCTRL;                         /** Receive control register */
  __IO uint32_t IE;                             /** UART interrupt enable */
  __I  uint32_t IP;                             /** UART interrupt pending */
  __IO uint32_t DIV;                            /** Baud rate divisor */
} UART_TypeDef;

#define UART0   ((UART_TypeDef *) UART_BASE)


int main(void) {

  char *ptr = "Hello, World!\n";
  size_t len = 14;
  int fd = 0;

  SYSCALL3(SYS_write, fd, (uintptr_t)ptr, len);

  

    uint64_t cmd = HTIF_TOHOST(0, 0, (0 << 1) | 0x1);
    for (;;) {
        fromhost = 0;
        tohost = cmd;
    }
  // _exit(0);

  return 0;
}
