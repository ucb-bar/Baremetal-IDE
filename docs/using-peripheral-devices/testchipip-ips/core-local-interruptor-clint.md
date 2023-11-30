# Core-Local Interruptor (CLINT)

## Introduction

The Core-Local Interruptor mode has one 64-bit timer register (`CLINT_MTIME`) and one or many sets of 32-bit machine-mode software interrupt-pending register (`CLINT_MSIP`) and 64-bit machine-mode timer compare register (`CLINT_MTIMECMP`). The number of machine-mode software interrupt-pending registers and machine-mode timer compare registers equals the number of harts in the system.



## CLINT main features

* 64-bit timer providing wall-clock time
* Configurable software local core interrupt generation to any hart
* Configurable software local time interrupt generation to any hart

## CLINT functional description

### MSIPx Functionality

MSIP register can be used to generate machine-mode software interrupts. Each MSIP register is a 32-bit wide register where the upper 31 bits are read-only and tied to 0. The least significant bit is reflected in the MSIP bit of the MIP CSR. On reset, each MSIP register is cleared to zero.&#x20;

Software interrupts are most useful for interprocessor communication in multi-hart systems, as harts may write each other’s MSIP bits to effect interprocessor interrupts.



### MTIME Functionality

MTIME is a 64-bit read-write register that contains the number of cycles counted from the RTC\_CLK input.&#x20;

A timer interrupt is pending whenever MTIME is greater than or equal to the value in the MTIMECMP register.&#x20;

The timer interrupt is reflected in the MTIP bit of the MIP CSR register.

On reset, MTIME is cleared to zero. The MTIMECMP registers are not reset.



### MTIMECMPx Configuration

Example program to set MTIMECMP value in RV32 systems.

```armasm
# New comparand is in a1:a0.
li t0, -1
la t1, mtimecmp
sw t0, 0(t1) # No smaller than old value. 
sw a1, 4(t1) # No smaller than new value. 
sw a0, 0(t1) # New value.
```



### CLINT registers

This section gives a detailed description of the CLINT registers.&#x20;

The peripheral registers can be written in word, half-word, or byte mode.

### Machine-mode software interrupt pending register n (MSIPn)

Address offset: `0x00 + 0x04 * n`

Reset value: `0x0000_0000`

<figure><img src="../../.gitbook/assets/image (4).png" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="98">Bits</th><th width="159">Name</th><th>Description</th></tr></thead><tbody><tr><td>0</td><td>MSIP</td><td><p>Machine-mode software interrupt pending for hart0.</p><p></p><p>This bit is controlled by software.</p><p>0: No software interrupt pending</p><p>1: Software interrupt pending</p></td></tr></tbody></table>



### Machine-mode time compare register n (MTIMECMPn)

Address offset: `0x4000 + 0x04 * n`

Reset value: `<previous value>`

<figure><img src="../../.gitbook/assets/image (5).png" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="102">Bits</th><th width="166">Name</th><th>Description</th></tr></thead><tbody><tr><td>63:0</td><td>MTIMECMP</td><td>Machine-mode time compare target.<br><br>These bits are controlled by software. Hardware will generate a machine-mode timer interrupt to hart0 when MTIME0 is greater than or equal to MTIMECMP0.</td></tr></tbody></table>



### Machine-mode time register (MTIME)

Address offset: `0xBFF8`

Reset value: `0x0000_0000`

<figure><img src="../../.gitbook/assets/image (6).png" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="102">Bits</th><th width="166">Name</th><th>Description</th></tr></thead><tbody><tr><td>63:0</td><td>MTIME</td><td><p>Machine-mode time.</p><p></p><p>These bits are controlled by software and hardware.</p></td></tr></tbody></table>









