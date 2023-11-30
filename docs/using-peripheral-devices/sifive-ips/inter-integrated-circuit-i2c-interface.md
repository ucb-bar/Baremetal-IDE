# Inter-Integrated Circuit (I2C) Interface

## Introduction

The I2C (inter-integrated circuit) bus interface handles communications to the serial I2C bus. It provides multi-master capability, and controls all I2C bus-specific sequencing, protocol, arbitration and timing. It supports Standard-mode (Sm), Fast-mode (Fm) and Fast-mode Plus (Fm+).



## I2C main features

* I2C bus specification compatibility:
  * &#x20;Slave and master modes
  * &#x20;Multimaster capability
  * Standard-mode (up to 100 kHz)
  * &#x20;Fast-mode (up to 400 kHz)
  * &#x20;Fast-mode Plus (up to 1 MHz)
  * 7-bit and 10-bit addressing mode



## I2C functional description

<figure><img src="../../.gitbook/assets/image (2).png" alt=""><figcaption><p>I2C block diagram</p></figcaption></figure>

### I2C signals

I2C communications require two pins: Serial Clock (SCL) and Serial Data (SDA):&#x20;

#### SCL (Serial Clock)

SCL is the bidirectional clock signal. When the device is in master mode, the device drives the SCL pin. When the device is in slave mode, the SCL pin is in input mode. Commonly, SCL is color-coded as <mark style="color:green;">green</mark>.&#x20;

#### SDA (Serial Data)

SDA is the bidirectional data signal.&#x20;

I2C operates in open-drain(OD) mode, which requires a pull-up resistor to the highest positive voltage supply on SCL and SDA pins. Commonly, SDA is color-coded as <mark style="color:yellow;">yellow</mark>.&#x20;



### I2C clock requirements

The Clock Generator generates an internal 4\*Fscl clock enable signal which triggers all synchronous elements in the Bit Command Controller. It also handles clock stretching needed by some slaves.



### Clock Prescaler Register

The prescaler value can only be changed when EN bit is disabled.

$$
prescale = \frac{f_{sys}}{5 \times f_{SCL}} - 1
$$



## I2C Communication Flow

Normally, a standard communication consists of four parts:&#x20;

1. START signal generation
2. Slave address transfer
3. Data transfer
4. STOP signal generation

<figure><img src="https://lh7-us.googleusercontent.com/uU-mOYImN0jz33U6zD_TaZvF4Ss3p6LzrEpAFnU_kQXyaja_vntJ-Cm5prO8TDLfQuyih7UHjo76r1T-itfs73dHY5ndUaIlrfZW81hrYfbk_3pMDIrN8piHxzX2OKN3H4aTRtl41-Z-E9XHgh6sgQQ" alt=""><figcaption><p>I2C frame</p></figcaption></figure>



### START signal

When the bus is free/idle, meaning no master device is engaging the bus (both SCL and SDA lines are high), a master can initiate a transfer by sending a START signal. A START signal is defined as a high-to-low transition of SDA while SCL is high. The START signal denotes the beginning of a new data transfer. A Repeated START is a START signal without first generating a STOP signal. The master uses this method to communicate with another slave or the same slave in a different transfer direction (e.g. writing to device to reading from device) without releasing the bus.&#x20;

The core generates a START signal when the STA bit in the Status Command Register (STAT\_CMD) is set and the RD or WR bits are set. Depending on the current status of the SCL line, a START or Repeated START is generated. \


### Slave address transfer

The first byte of data transferred by the master immediately after the START signal is the slave address.&#x20;

This is a seven-bit calling address followed by a R/W bit. The R/W bit signals the slave data transfer direction. No two slaves in the system can have the same address. Only the slave with an address that matches the one transmitted by the master will respond by returning an acknowledge bit by pulling the SDA low at the nineth SCL clock cycle.&#x20;

The core treats a Slave Address Transfer as any other write action. Store the slave device’s address in the Data Register (DATA) and set the WR bit in the Status Command Register (STAT\_CMD). The core will then transfer the slave address on the bus.

### Data transfer

Once successful slave addressing is achieved, the data transfer can proceed on a byte-by-byte basis in the direction specified by the R/W bit sent by the master. Each transferred byte is followed by an acknowledge bit on the nineth SCL clock cycle.&#x20;

If the slave signals a No Acknowledge, the master can generate a STOP signal to abort the data transfer or generate a repeated START signal and start a new transfer cycle.&#x20;

If the master, as the receiving device, does not acknowledge the slave, the slave releases the SDA line for the master to generate a STOP or repeated START signal.&#x20;

For writing data to a slave, store the data to transmit in the Data Register (DATA) and set the WR bit in the Status Command Register (STAT\_CMD). For reading data from a slave, set the RD bit in the Status Command Register (STAT\_CMD). During a transfer the core set the TIP flag, indicating that a Transfer is In Progress and the peripheral is busy. When the transfer is done the TIP flag is reset, the IF flag set and, when enabled, an interrupt is generated. The Data Register (DATA) contains valid data after the IF flag has been set. The user may issue a new write or read command when the TIP flag is reset.



### STOP signal

The master can terminate the communication by generating a STOP signal. A STOP signal is defined as a low-to-high transition of SDA while SCL is high.



### Mode of Operation

In Master mode, the I2C interface initiates a data transfer and generates the clock signal. A serial data transfer always begins with a START condition and ends with a STOP condition. Both START and STOP conditions are generated in master mode by software.&#x20;

In Slave mode, the interface is capable of recognizing its own addresses (7 or 10-bit), and the general call address. The general call address detection can be enabled or disabled by software.

Data and addresses are transferred as 8-bit bytes, MSB first. The first byte(s) following the START condition contain the address (one in 7-bit mode, two in 10-bit mode). The address is always transmitted in Master mode. A ninth clock pulse follows the 8 clock cycles of a byte transfer, during which the receiver must send an acknowledge bit to the transmitter.

\


<figure><img src="https://lh7-us.googleusercontent.com/NAj-9oCFzxQQSDTLi4P0hzmLJtE2nXFoiVtY5U5wLoHoMy_EkFglvKWA1iqYGFFLfTUppy7KDiaSdR3QURhnR-ZAd7AZj2TnQJylSWQuRvd8Vjs9y0UaKjV101I0jeGOAoQ5AkP7mVetRMekHVjLevs" alt=""><figcaption><p>I2C conditions</p></figcaption></figure>



### Data Reception

1. generate start signal
2. write slave address + write bit
3. receive acknowledge from slave
4. write memory location
5. receive acknowledge from slave
6. generate repeated start signal
7. write slave address + read bit
8. receive acknowledge from slave
9. read byte from slave
10. write no acknowledge (NACK) to slave, indicating end of transfer
11. generate stop signal



### Data Transfer

1. generate start command
2. write slave address + write bit
3. receive acknowledge from slave
4. write data
5. receive acknowledge from slave
6. generate stop command



## I2C registers

This section gives a detailed description of the I2C registers.&#x20;

The peripheral registers can be written in word, half-word or byte mode.

### Prescaler low register (PRESCAL\_LO)

Address offset: `0x00`

Reset value: `0x0000_00FF`\


<figure><img src="https://lh7-us.googleusercontent.com/m01nHlWCFbZO48A0r-joH5TZowe37UlcZkZduU6gvb6TkMwJL6S7C1gV8oJgFY9eTOcJgaaVeUaLQ5LYsIqLkBwW2mwrGNMbk3avFW8OVsdco0GbP_XTJl1RKVGOGh0xxJXBfzTBR36mkibqFsjRnX8" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="98">Bits</th><th width="159">Name</th><th>Description</th></tr></thead><tbody><tr><td>7:0</td><td>PRESCAL_LO</td><td><p>Lower part of the prescaler value.</p><p></p><p>These bits are set and cleared by software.</p></td></tr></tbody></table>



### Prescaler high register (PRESCAL\_HI)

Address offset: `0x04`

Reset value: `0x0000_00FF`

<figure><img src="https://lh7-us.googleusercontent.com/BiOfn3dZFBtOKvsFfai2NMvuIiYspNxIKfY3yiQSIfW6z5-nPvG0gF3ZkNSagkuQi8hCTa3_1KXTU_G9HpCZaz8yRNovINQlrFpMZIn4K1CVqpx4SEUGyQecachhAIGS4rabp8SZKA5q5rowpy5ear4" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="98">Bits</th><th width="159">Name</th><th>Description</th></tr></thead><tbody><tr><td>7:0</td><td>PRESCAL_HI</td><td><p>Higher part of the prescaler value.</p><p></p><p>These bits are set and cleared by software.</p></td></tr></tbody></table>



### Control register (CTRL)

Address offset: `0x08`

Reset value: `0x0000_0000`

<figure><img src="https://lh7-us.googleusercontent.com/5mz38sfhlr-zd2z-avyEOowEh-k49p7klEUwj-4EHLOMNYv7ZpMeii-vvwyHcED1OGl0RKpIJE_P1cv8_bBzScFSpwk83L6jXzW6g5PDj3xfrgwI2qOhuHrXG9TVtpBXjzPdoObmg5EbawjYbCE5kq0" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="98">Bits</th><th width="159">Name</th><th>Description</th></tr></thead><tbody><tr><td>7</td><td>EN</td><td><p>I2C core enable bit.</p><p></p><p>These bits are set and cleared by software.<br>0: core disabled</p><p>1: core enabled</p></td></tr><tr><td>6</td><td>IEN</td><td><p>I2C interrupt enable bit.<br><br>These bits are set and cleared by software.<br>0: interrupt disabled</p><p>1: interrupt enabled</p></td></tr></tbody></table>



### Data register (DATA)

Address offset: `0x0C`

Reset value: `0x0000_0000`

<figure><img src="https://lh7-us.googleusercontent.com/WdxuavsU52JXBwkfHH3cTkB5z9vkKYK9n3PK-KzaA4VTSkXLLOvqfkOwjN6vNFVf42tRZenVIYFPjaTe5bCkNV7vEcnm7ODFstHHBS2XfU05o0pJURiTUGncjgf5BnUamq_tZNs7smClwmUUb_NwALs" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="98">Bits</th><th width="159">Name</th><th>Description</th></tr></thead><tbody><tr><td>7:0</td><td>DATA</td><td><p>Data field.</p><p></p><p>When read, returns the last data received by I2C RX; when written, puts the value into I2C TX buffer. When used with I2C START condition, the LSB is I2C R/W mode bit.</p><p><br>These bits are set and cleared by software.<br></p><p>Note that writing and reading are operating on different fields.</p><p><br>0: write command</p><p>1: read command</p></td></tr></tbody></table>



### Status command register (STAT\_CMD)

Address offset: `0x10`

Reset value: `0x0000_0000`

<figure><img src="https://lh7-us.googleusercontent.com/10QTP6BE-u1hYpaQe8HlgWxoNiGiZPf9-g-SXiCVjsBO9eGICBoPWv5q00tUZm5kCY66fEgbMd7qy0n97HQgsOMLhLOnzuhsEeZwMElTnd1Hn0IF9K86lskng8hxMAsKXxn_zOEo1ILC4X1BlQBak_o" alt=""><figcaption></figcaption></figure>

<table data-full-width="true"><thead><tr><th width="98">Bits</th><th width="159">Name</th><th>Description</th></tr></thead><tbody><tr><td>7</td><td>RXACK / STA</td><td><p>Receive acknowledgment status / start bit.<br></p><p>When read, return the received acknowledge status from the slave; when written, generate (repeated) start condition</p><p><br>These bits are set by software, and set and cleared by hardware.</p><p><br>When read: <br>0: acknowledge received (ACK) </p><p>1: no acknowledge received (NACK) </p><p><br>When written: </p><p>0: do not generate START (no effect) </p><p>1: generate START</p></td></tr><tr><td>6</td><td>BUSY / STO</td><td><p>Busy / stop bit.</p><p><br>When read, return the bus status; when written, generate stop condition These bits are set by software, and set and cleared by hardware.</p><p>These bits are set and cleared by software and hardware.</p><p><br>When read: </p><p>0: detected STOP condition</p><p>1: detected START condition<br></p><p>When written:</p><p>0: do not generate STOP (no effect)</p><p>1: generate STOP</p></td></tr><tr><td>5</td><td>RD</td><td><p>Initiate a read from slave operation.<br><br>When written: </p><p>0: do nothing</p><p>1: start a read operation</p></td></tr><tr><td>4</td><td>WR</td><td><p>Initiate a write to slave operation.<br></p><p>When written: </p><p>0: do nothing</p><p>1: start a write operation</p></td></tr><tr><td>3</td><td>ACK</td><td><p>Send acknowledge bit</p><p></p><p>When written: </p><p>0: do nothing</p><p>1: send acknowledge bit</p></td></tr><tr><td>1</td><td>TIP</td><td><p>Indicate if a transfer is in progress.</p><p><br>When read: </p><p>0: transfer is complete</p><p>1: transfer in progress</p></td></tr><tr><td>0</td><td>IF</td><td><p>Indicate if an interrupt is pending</p><p><br>Will cause a processor interrupt request if IEN bit is set</p><p><br>When read: </p><p>0: transfer is complete</p><p>1: transfer in progress</p></td></tr></tbody></table>



