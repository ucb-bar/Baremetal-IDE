# Common Issues in Chip Bringup



## Stack and Heap Size Error

Error message:

```bash
relocation truncated to fit: R_RISCV_GPREL_I in .stack
```

Solution:&#x20;

increase stack size or heap size defined in linker.





## Hart unexpectedly reset

Error message:

```bash
> openocd -f .\bearlyml.cfg
Open On-Chip Debugger 0.11.0-rc1+dev (SiFive OpenOCD 0.10.0-2020.12.1)
Licensed under GNU GPL v2
For bug reports:
        https://github.com/sifive/freedom-tools/issues
Info: clock speed 100 kHz
Info: JTAG tap: riscv.cpu tap/device found: 0x00000001 (mfg: 0x000 (<invalid>), part: 0x0000, ver: 0x0)
Info: datacount=2 progbufsize=16
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!
Info: Hart 0 unexpectedly reset!

```

This issue happened when testing BearlyML 22 using jumper wire harness on the breakout board. No solution was found. However, the issue disappeared when we moved to a more complete demo board, so we suspect this might be due to some signal integrity issue.

Solution:

check harness





## Unable to halt hart

error message:

```bash
> openocd -f .\bearlyml.cfg
Open On-Chip Debugger 0.11.0-rc1+dev (SiFive OpenOCD 0.10.0-2020.12.1)
Licensed under GNU GPL v2
For bug reports:
        https://github.com/sifive/freedom-tools/issues
Info: clock speed 100 kHz
Info: JTAG tap: riscv.cpu tap/device found: 0x00000001 (mfg: 0x000 (<invalid>), part: 0x0000, ver: 0x0)
Info: datacount=2 progbufsize=16
Error: unable to halt hart 0
Error:   dmcontrol=0x80010001
Error:   dmstatus =0x00030ca2
Error: Fatal: Hart 0 failed to halt during examine()
Warn: target riscv.cpu0 examination failed     
```

solution:

check if core is bring out of reset.

check if core is powered

check if system clock is supplied





## JTAG Scan chain all ones

Device is not connected to the debugger. Check connection to the DUT device.

<pre class="language-bash"><code class="lang-bash">(base) tk@TK-Avenue-Ubuntu:~/Desktop/HAL/jtag$ openocd -f osciarty.cfg 
Open On-Chip Debugger 0.11.0-rc1+dev (SiFive OpenOCD 0.10.0-2020.12.1)
Licensed under GNU GPL v2
For bug reports:
        https://github.com/sifive/freedom-tools/issues
Info : clock speed 2000 kHz
Error: JTAG scan chain interrogation failed: all ones
Error: Check JTAG interface, timings, target power, etc.
Error: Trying to use configured scan chain anyway...
Error: riscv.cpu: IR capture error; <a data-footnote-ref href="#user-content-fn-1">saw 0x1f not 0x01</a>
Warn : Bypassing JTAG setup events due to errors
Error: Unsupported DTM version: 15
Warn : target riscv.cpu examination failed
Info : starting gdb server for riscv.cpu on 3333
Info : Listening on port 3333 for gdb connections
Error: Target not examined yet

Error: Unsupported DTM version: 15
</code></pre>



## Default SPI Frames

default SPI frame is MSB first, SCLK is default low, and data is latched on SCLK rising edge (SPI mode 0)



## Error when using sprintf()

Error:

Program cannot run when using \`sprintf()\` with variables

Solutions:

Could due to the following reasons

* program being too large with the additional libraries
* instructions could be corrupted during SPI transmission
* malloc() implementation issue

\






Size should be 2^2 for either, FESVR is not very thoroughly tested on 32 bit systems.





VCU118

DO NOT PRESS \`PROGRAM\` BUTTON!!! This will erase the image on FPGA



[^1]: all ones here
