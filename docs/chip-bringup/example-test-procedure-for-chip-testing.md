# Example Test Procedure for Chip Testing

## 1. Power

Check if there is short between all power domains and GND, and also between each power domains.

If no short is observed, power up IO power domain (normally labeled VDD\_IO) and digital power domain (normally labeled VDD\_D). Observe the current consumption. Normally on a quad-core chip, IO should draw < 10 mA, and digital domain should draw < 200 mA.



## 2. Clock

Supply clock to the digital domain. Start the test from a slow frequency, 1 kHz is a good value. Make sure the voltage level of the supplied clock matches the chip's specification. If there is an input IO cell, the voltage should be equal to VDD\_IO; if the clock is directly fed into the system (commonly seen on mixed-signal chips with analog chiptop, or high-speed clocks), the voltage should be equal to VDD\_D.

If there is an external clock output pin, observe the clock pattern on the pin. The pin should output either the same or some divided fraction of the input clock.



## 3. Memory subsystem

With power and clock supplied, observe if there is clock output on TL\_CLK pin.&#x20;



## 4. JTAG

Connect JTAG and see if the debugger can probe the core.



## 5. Flash

