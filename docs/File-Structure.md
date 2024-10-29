# File Structure

## Overview

This document contains details about what the core components are, what they contain, and how they are organized in the file structure.

## Organization

The code files are organized into following groups

The `platform` group provides hardware description for supported targets.

The `driver` group includes peripheral drivers, risc-v architecture specific files, and others. This group contains information about and provides low-level access to the underlying hardware. In the case of rv, it contains hardware-agnostic code and utilities on the RISC-V platform. These components may have dependencies on each other within the group, but outside dependencies should be minimized. The reason for this approach is that these components are fundamental, and many other components may require them. Ideally, the dependency relationship only goes one way, making it easier for this group to be usable in other projects.

The `glossy` layer provides the necessary baremetal functionalities, including startup routines, system calls, interrupt and exception handling, and basic memory management.

The `lib` operates at a higher level than the driver group, including more advanced functionalities like freertos, logging, and threading. Like the driver group, circular dependencies within this group are allowed, and these components can have dependencies on the driver group. The library components represent essential software mechanisms for building other components.

The dependency graph is as follows:

```bash
driver <- platform <- glossy <- lib <- application
```


## Descriptions

The following is a short description of the components mentioned above.

### `glossy` layer

Contains the fundamentally low-level architecture operations and definitions, including those for customizations (e.g. RoCC command templates). Also contains hardware-agnostic definitions, constants, macros, utilities, 'pure' and/or algorithmic functions that is useable by all other components (that is, barring there being a more appropriate component to put them in).

Example:

- `SET_BITs()` `CLEAR_BITS()` and other bit manipulation utilities

- `READ_CSR()`, `WRITE_CSR()`

- `LREG` and `SREG` for generic load/store definitions that is compatible with assembly compilers

- RISC-V interrupt types `InterruptType`

- `get_hart_id()`

- basic newlib syscalls that user can override


### Peripheral drivers (`driver/<ip-supplier>/<device>/`)

Contains the hardware abstraction layer and low-level operation implementations for the various peripherals. The low-level functions provides functions to device-specific register-level manipulations; the hardware abstraction provide operations one level above this, grouping these low-level functions into routines that achieve a meaningful action or state of the peripheral.

Example:

- `OK`, `ERROR`, `SET`, `LOW`, and other state and error definitions

- `UART_TXDATA_DATA_POS`, `UART_TXDATA_DATA_MSK`, and other peripheral-specific register bit definitions

- `void uart_init(UART_Type *UARTx, UART_InitType *UART_init)` and other peripheral initialization routines

- `Status uart_receive(UART_Type *UARTx, uint8_t *data, uint16_t size, uint32_t timeout)` and other functions to invoke peripheral procedures.

- Default definitions for basic peripherals like `CLINT`, `PLIC`, `UART0`, and `GPIOA` at their default memory location


Contains description of the underlying hardware: memory layout structure, memory addresses, power, clocking, and interrupt settings, etc.

### `lib` components

Contains provided or third-party libraries for more advanced functionalities. 

#### pthread

A fake pthread implementation that achieves hart-based multithreading.

#### baremetal-nn

Support for neural network workloads.


