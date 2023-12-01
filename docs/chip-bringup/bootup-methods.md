# Bootup Methods



Boot mode select pin can select between tethered (TSI or JTAG) and standalone boot (SPI flash)



## 1. JTAG

JTAG basic functionalities: IRCapture, IDCODE, BYPASS working √ (some instabilities)

RISC-V related DTM & DMI register value read all zeros ×

Cause: some typo when hand-routing verilog, and thus messed up with something; or something wrong with cross-clock domain parts



Another tethered boot method, in case there are problems with the TSI widget

We can bitbang programs over the JTAG interface

Into the 16KB scratchpad for small programs/RTOSes

Into DRAM (if TSI does work)

Manually bring the core out of idling with a software interrupt (through the CLINT)

Anything can be used as the host. Laptop with USB-to-JTAG adapter, or any microcontroller

RISC-V port of OpenOCD works, GDB debugging is good

\




## 2. QSPI Flash

Standalone or Tethered w/ TSI

Execute in Place (XiP)

Copy program from flash into scratchpad (16KB) / DRAM (4GB, avoid collision with  FPGA Tether)

\


Currently we just have a bypass for sim, still need a functional image that is small enough to fit on SPI flash.&#x20;

Also trying to get SPI Flashing access from FPGA incase chip’s JTAG breaks

\


\




## 3. Serialized TileLink





We use Tethered Serial Interface (TSI) as a transport for communication between the host and DUT.

Allows reading and writing arbitrary locations in system memory

A way to send memory requests to off-chip DRAM over a serial link

<figure><img src="https://lh7-us.googleusercontent.com/SsxSl2m3CwRyktPZPJtj_byKjwRr3C2mLelRFtRdPxQNjW2Fp-SGplU_ed-mk64kygWcZOPuYgQaen9w3rI8CUoL7yTVytUCaXlIcJmdxlt7CxpCd9ZuwPrYL_6Y4R8Nu1zm1ojydvw4O7MVqZUQVypuYg=s2048" alt=""><figcaption></figcaption></figure>

Currently the fastest way to load a program

Steps:

1. The host writes the program into memory over TSI
2. May bring other cores out of reset
3. Sends an interrupt signal to the core to start it running

Via the msip memory-mapped I/O register in the CLINT



Less wires than AXI or DDR, so good when I/O limited

Envisioned as an intra-chip interconnect

The TileLink protocol assumes reliable messages, which may not be the case when going off-chip

No notion of retries or timeouts in the protocol, managers must be infinitely patient

No cycles allowed



\


<figure><img src="https://lh7-us.googleusercontent.com/m4IHUrPnz6lXeZgAieoKmUDc-cqTwsW2Req9ADysqEipbh0bozsKDESk0st1rEwzDbdwVWMZl2ZzX8CBRhoD4oLylWsbmtQ6Yp1F3nm2xX2_2XSdLsxM3INVewqvYftSjYp3y7vgYuU-O9xEuCdEITMn=s2048" alt=""><figcaption></figcaption></figure>



Currently can get first TL packet of FPGA -> OSCIBEAR communication, but OSCIBEAR hungs, not responding to the packet, and thus block the TL interface

TODO: need to look into code to interpret the packet, and verify that FPGA is sending correct signal

\
