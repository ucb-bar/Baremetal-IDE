# Adding Support for New SoCs

To add support for new SoCs, create a new folder under `/bsp` directory with a unique `chip_name`.

The `chip_name` should be all lower-case, and ideally consist only \[a-z] characters. No space is allowed.



Under the `/bsp/<chip_name>` folder, create directories and files following the existing chip support file structure.



Then, edit `/bsp/<chip_name>/inc/<chip_name>.h` file. Edit the "Memory Map Definition" section to match the SoC's memory layout. Also, update "Peripheral Pointer Definition" accordingly.&#x20;

If new peripheral device is present in the SoC, add a new TypeDef struct in the "Peripheral Struct Definition" section, and add the corresponding bit position and mask definitions in the "Peripheral Bit Field Definition" section.



Update `/bsp/<chip_name>/debug/<chip_name>.cfg` file to match the hart configuration on the SoC.



Finally, edit the clock frequency and mtime scale in `/bsp/<chip_name>/inc/<chip_name>_hal.h` to match the value provided to the SoC.



