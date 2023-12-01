# JTAG





## JTAG State Machine

JTAG will reset after TMS == 1 for 5 or more cycles

<figure><img src="../.gitbook/assets/image (2).png" alt=""><figcaption></figcaption></figure>

From these [two](https://www.fpga4fun.com/JTAG3.html) [sources](https://www2.lauterbach.com/pdf/training\_jtag.pdf), it seems that JTAG will only sample TDO AFTER entering shift IR/DR mode

E.g. it will not include the data at transition from capture to shift

Similarly, TDO should be sample by master device AFTER entering shift IR/DR mode, and on the same clock cycle as writing data to it.





## JTAG Bit Timing

JTAG target device will sample TDI and TMS at rising TCK edge, and change TDO state at falling TCK edge.

Thus, for us, we need to initialize on TCK == LOW, set TDI and TMS before rising edge, delay .25 period, raise TCK, delay .5, lower TCK, delay .25, sample TDO.

([Reference](https://developer.arm.com/documentation/dui0499/k/arm-dstream-target-interface-connections/signal-descriptions/jtag-port-timing-characteristics))



<figure><img src="../.gitbook/assets/image.png" alt=""><figcaption><p>a JTAG bit</p></figcaption></figure>



<figure><img src="../.gitbook/assets/image (1).png" alt=""><figcaption><p>JTAG communication frames</p></figcaption></figure>





## Low-level JTAG Debugging

### Read IDCODE

JTAG should reset with either capture IR or IDCODE mode

We can try to read IDCODE first

Seqence:

1. R Test logic reset
2. 0 Run idle
3. 1 Select DR
4. 0 Capture DR
5. 0 Shift DR
6. LOOP 32: 0 Shift DR -> TDO

\


### Test DR Width

We need the exact width of DR for following tests, so it’s better we test it here

We will supply a bunch of zeros to the DR chain to clear out everyone, and supply one 1 in.&#x20;

We then keep read the DR chain until this 1 emerges at the TDO, then, we count the cycles to get the bit width of the DR chain.

On Chipyard SoCs, DR width should be 32



### Test IR Width

On Chipyard SoCs, IR width should be 5



### Test BYPASS

First we go to shift IR, and input a bunch of 1s to put everything on chain into BYPASS mode.

After updating IR and going back to idle, we then go to shift DR and shift in a 1. (DR in BYPASS should reset to be 0)

Then we shift DR until we receive 1 at TDO.

If there’s only one device on chain, we should be getting 1 immediately next cycle, and the rest are all 0s (because then we begin shift in 0s)

We should receive BYPASS after 1 cycle

\
\






