#ifndef __RV_CORE_H
#define __RV_CORE_H

#include <stdint.h>

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

/* ================ basic types ================ */
#define SET_BITS(REG, BIT)                    ((REG) |= (BIT))
#define CLEAR_BITS(REG, BIT)                  ((REG) &= ~(BIT))
#define READ_BITS(REG, BIT)                   ((REG) & (BIT))
#define WRITE_BITS(REG, CLEARMASK, SETMASK)   ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))

#define READ_CSR(REG) ({                          \
  unsigned long __tmp;                            \
  asm volatile ("csrr %0, " REG : "=r"(__tmp));  \
  __tmp; })

#define WRITE_CSR(REG, VAL) ({                    \
  asm volatile ("csrw " REG ", %0" :: "rK"(VAL)); })

#define SWAP_CSR(REG, VAL) ({                     \
  unsigned long __tmp;                            \
  asm volatile ("csrrw %0, " REG ", %1" : "=r"(__tmp) : "rK"(VAL)); \
  __tmp; })

#define SET_CSR_BITS(REG, BIT) ({                 \
  unsigned long __tmp;                            \
  asm volatile ("csrrs %0, " REG ", %1" : "=r"(__tmp) : "rK"(BIT)); \
  __tmp; })

#define CLEAR_CSR_BITS(REG, BIT) ({               \
  unsigned long __tmp;                            \
  asm volatile ("csrrc %0, " REG ", %1" : "=r"(__tmp) : "rK"(BIT)); \
  __tmp; })

#define rdtime() read_csr(time)
#define rdcycle() read_csr(cycle)
#define rdinstret() read_csr(instret)

typedef enum {
  RESET = 0UL,
  SET   = !RESET,

  DISABLE = RESET,
  ENABLE  = SET,
  
  LOW   = RESET,
  HIGH  = SET,
} State;

typedef enum {
  OK = 0U,
  ERROR,
  BUSY,
  TIMEOUT
} Status;

#endif /* __RV_CORE_H */
