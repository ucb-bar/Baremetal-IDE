# Directory Structure

### `/bsp`

BSP stands for "Board-Specific Packages". This folder stores board-specific files.&#x20;

The directory `bsp/<chip_name>` stores the files specific to this SoC.&#x20;

Meanwhile, the common files that is shared across SoCs such as debugger configurations, are stored under `bsp/common`.&#x20;



### `/bsp/<chip_name>/debug`

This folder stores the configuration files used during SoC debugging.&#x20;

`<chip_name>.cfg` contains the necessary information needed by the JTAG debugger about the hart and memory configuration of the SoC.&#x20;

`<chip_name>.svd` file enables interactive live register view in VS Code.



### `/bsp/<chip_name>/inc`

This folder stores the header file of the SoC HAL (hardware-abstraction layer) library.

`<chip_name>.h` contains the supported interrupt type, interrupt IDs, the memory layout of the peripheral devices, the memory map, and the bit definitions of the register of peripheral devices. It defines almost all the necessary information about the SoC. Therefore, it is possible to write low-level baremetal code with only this file when performance and optimization are needed.

`<chip_name>_hal.h` contains the HAL library definitions. User can select which peripheral device HAL library to include by adding or removing the include definitions. This file also defines the system clock frequency and real-time time scale.

`rv_**.h` contains RISC-V specific definitions.



### `/bsp/<chip_name>/src`

This folder stores the source file of the SoC HAL (hardware-abstraction layer) library.

`<chip_name>_hal_system.c` contains definitions to available traps on the SoC. User can override these functions in the application code to perform interrupt handling.



### `/bsp/<chip_name>/startup`

This folder stores the startup procedure of the SoC.



### `/core`

This folder stores the application-specific code. Users can implement the application functionality either directly in `main.c`, or create hierarchies within this folder for more complicated applications.



### `/lib`

This folder stores third-party libraries.

