# RISC-V Programming

## What is a RISC-V Toolchain? <a href="#what-is-a-risc-v-toolchain" id="what-is-a-risc-v-toolchain"></a>

The `riscv64-unknown-elf-xxx` toolchain is specifically designed for the RISC-V instruction set architecture (ISA), and it’s used for developing bare-metal and applications for running on RISC-V targets. This toolchain typically runs on the host machine (e.g., an x86-based PC) to produce code that will run on a RISC-V machine (e.g., a RISC-V based SoC). This process is known as cross-compilation.

Breaking down the name riscv64-unknown-elf-xxx:

`riscv64`: This refers to the RISC-V architecture with a 64-bit address space. RISC-V is an open-standard ISA that’s been gaining traction in both academia and industry due to its flexibility and modularity.

`unknown`: This indicates that the toolchain is not tied to a specific vendor or platform. It’s a generic term used to describe that the toolchain can be used across different RISC-V implementations.

`elf`: Stands for Executable and Linkable Format. It’s the format used for the compiled binaries. ELF is a common binary file format that’s used on many Unix systems, including Linux.

`xxx`: Typically, this would be replaced by specific tools within the toolchain, like:

* gcc: The GNU Compiler Collection for RISC-V, also known as **the compiler**.
* gdb: The GNU Debugger for debugging RISC-V applications, used for debugging programs.
* ld: The **linker**.
* objdump: A utility that displays information about object files.
* … and so on

## What is OpenOCD? <a href="#what-is-openocd" id="what-is-openocd"></a>

OpenOCD, which stands for Open On-Chip Debugger, is a free, open-source software that provides debugging, in-system programming, and boundary-scan testing for embedded systems. It’s used to program and debug embedded systems using JTAG or SWD (Serial Wire Debug) interfaces.

#### Main functionalities <a href="#main-functionalities" id="main-functionalities"></a>

**Debugging**: Through OpenOCD, developers can control the JTAG state machine to run a program on an embedded target, halt the target at a breakpoint, and then inspect or modify variables to debug the software.

**Flashing**: OpenOCD can be used to load program (also called “flash”) onto either internal or external Flash memory of the SoC.

**Remote Debugging**: OpenOCD can function as a GDB (GNU Debugger) server. This means developers can use GDB or a GDB-compatible front-end to remotely debug an application running on an embedded target through OpenOCD.

**Scriptable Interface**: OpenOCD provides a flexible and scriptable interface, which is valuable for automated testing and custom debugging routines.

## Setting up RISC-V Environment <a href="#setting-up-risc-v-environment" id="setting-up-risc-v-environment"></a>

In this tutorial, we will set up the RISC-V environment by installing necessary toolchains and other softwares.

{% content-ref url="http://127.0.0.1:5000/o/QxuKWlObc6aI0kOw9xus/s/VJt4O1JTDNktDRwjb6Q8/" %}
[Berkeley Humanoid Manual](http://127.0.0.1:5000/o/QxuKWlObc6aI0kOw9xus/s/VJt4O1JTDNktDRwjb6Q8/)
{% endcontent-ref %}
