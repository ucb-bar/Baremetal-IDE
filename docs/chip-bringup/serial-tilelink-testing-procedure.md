# Serial TileLink Testing Procedure

## TileLink and Serial TileLink <a href="#1-tilelink-and-serial-tilelink" id="1-tilelink-and-serial-tilelink"></a>

### TileLink <a href="#11-tilelink" id="11-tilelink"></a>

TileLink is a bus protocol used by Chipyard’s memory subsystem. Please read and understand how Channel A and D works in Section 3 of the [TileLink Spec](https://starfivetech.com/uploads/tilelink\_spec\_1.8.1.pdf), this information will be useful in following sections.

### Serial TileLink <a href="#12-serial-tilelink" id="12-serial-tilelink"></a>

Serial TileLink (SerialTL) protocol is an implementation of HTIF that is used to send commands to the RISC-V DUT. These SerialTL commands are simple R/W commands that are able to access the DUT’s memory space. During test, the host machine sends SerialTL commands through an USB adapter to DUT. The DUT then converts the SerialTL command into a TileLink request. This conversion is done by the SerialAdapter module (located in the `generators/testchipip` project). After the transaction is converted to TileLink, the TLSerdesser (located in `generators/testchipip`) serializes the transaction and sends it to the chip (this TLSerdesser is sometimes also referred to as a digital serial-link or SerDes). Once the serialized transaction is received on the chip, it is deserialized and masters a TileLink bus on the chip which handles the request.

TLDR: it very simply serializes TileLink packets into at least 7 wires, with configurable width of data wires and utilizes a [ready-valid interface](https://inst.eecs.berkeley.edu/\~cs150/Documents/Interfaces.pdf) to transfer data in both directions.

7 wires (directions are from the perspective of testchip):

* Clock signal
  * `TL_CLK` (from testchip)
* testchip to FPGA link
  * `TL_OUT_VALID` (from testchip)
  * `TL_OUT_READY` (from FPGA)
  * `TL_OUT_BITS` (from testchip)
* FPGA to testchip link
  * `TL_IN_VALID` (from FPGA)
  * `TL_IN_READY` (from testchip)
  * `TL_IN_BITS` (from FPGA)

As you can see, this is a very simple protocol with little hardware overhead. But there are downsides, because TileLink is infinitely patient and has no timeout. So if a packet is lost, corrupted, unrecoverable, or not replied, the whole chip will freeze. Watch out for chip-wide freezes when debugging, it is likely a TileLink issue when communicating over SerialTL.



An example SerialTL frame

<figure><img src="../.gitbook/assets/image (9).png" alt=""><figcaption></figcaption></figure>



### Loading Program over SerialTL <a href="#2-loading-program-over-serialtl" id="2-loading-program-over-serialtl"></a>

As we’re still figuring out how to have 20+ people accessing only two OsciBear on one lab bench with one power supply & clock generator, we’ll simply explain how our setup works for now & plan to expand this access down the road. The following labs are experimental procedures of remotely debugging OsciBear.



### SerialTL Packets on Osci <a href="#21-serialtl-packets-on-osci" id="21-serialtl-packets-on-osci"></a>

Please complete the following table using SerialTL code snippet from [OsciBear’s verilog top](https://github.com/ucberkeley-ee290c/fa22/blob/main/oscibear/sp21/chipyard.TestHarness.EE290CBLEConfig.top.v), and save it for your future reference. Signal are ordered exactly how SerialTL serializes/deserializes, do not try to change the ordering.



```verilog
module GenericSerializer(clock, reset, io_in_ready, io_in_valid,
     io_in_bits_chanId, io_in_bits_opcode, io_in_bits_param,
     io_in_bits_size, io_in_bits_source, io_in_bits_address,
     io_in_bits_data, io_in_bits_corrupt, io_in_bits_union,
     io_in_bits_last, io_out_ready, io_out_valid, io_out_bits);
  input clock, reset, io_in_valid, io_in_bits_corrupt, io_in_bits_last,
       io_out_ready;
  input [2:0] io_in_bits_chanId, io_in_bits_opcode, io_in_bits_param;
  input [3:0] io_in_bits_size, io_in_bits_source;
  input [31:0] io_in_bits_address;
  input [63:0] io_in_bits_data;
  input [7:0] io_in_bits_union;
  output io_in_ready, io_out_valid, io_out_bits;
```





### Testing if SerialTL is Responding Correctly <a href="#22-testing-if-serialtl-is-responding-correctly" id="22-testing-if-serialtl-is-responding-correctly"></a>

To test if SerialTL is working, read out the BootROM and confirm the contents are correct. To further confirm, we constructed a SerialTL packet to write some code into the DTIM (scratchpad) and read it back. Let’s try this process by hand, but there are two caviats we found out:

1. To send/recieve a SerialTL packet, we construct/deconstruct the message according to TileLink and _flip the bit order (reverse every bit)_.
2. OsciBear is 32 bits, but the data width is 64, which means _the upper 32 bits of data are always 0_ on this chip, and we need to change the `mask` field accordingly.



#### **SENDING A SERIALTL READ REQUEST PACKET (A CHANNEL)**

Construct an A Channel SerialTL packet yourself by trying to read the BootROM at `0x0001_0000`. Remember both caviats above and assume `last` will always be high. You can express this in verilog or in other language that you feel comfortable in, and given a handy imaginary `$reverse(in)` block to reverse the bit order.



### Running code at DTIM <a href="#22-running-code-at-dtim" id="22-running-code-at-dtim"></a>

The full BootROM dump for Osci is [here](https://github.com/ucberkeley-ee290c/chipyard-osci-sky130/blob/master/generators/chipyard/src/main/scala/ee290c/bootrom/bootrom.rv32.dump), beware that the future chips are slightly different. A BootROM is a small read only memory consisting of boot code that jumps to various PC addresses. Basically, the core with hartid=0 will be in a loop checking for CLINT interrupts at `0x0200_0000`, the msip register. After finishing transfer of our code, we set the msip register to 1 through SerialTL, the BootROM will jump to `<boot_core_hart0>` and begin executing at `0x8000_0000`, our DTIM or scratchpad address. The jump is done by setting mepc and calling mret, for more about interrupts see [RISC-V Interrupts by Krste Asanovic](https://riscv.org/wp-content/uploads/2016/07/Tue0900\_RISCV-20160712-Interrupts.pdf).



