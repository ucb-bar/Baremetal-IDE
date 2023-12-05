# Sanity Check UART Program

This program is useful for checking minimal chip signs of life



startup.S

```armasm
.section .text, "ax", @progbits
.globl _start
_start:
  csrr a0, mhartid
  li a1, 0x10020000
  li a2, 1
  sw a2, 8(a1)
  addi a0, a0, 0x41
loop:
  sw a0, 0(a1)
  j loop
```



linker.ld

```linker-script
SECTIONS {
  ROM_BASE = 0x8000000; /* ... but actually position independent */
  . = 0x8000000;
  .text : {
    *(.text)
  }
}
```
