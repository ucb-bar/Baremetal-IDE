# Code Style

This document defines the code style and naming conventions for Baremetal-IDE.

For C/C++ code, we generally follow the Google C++ Style Guide, unless otherwise specified. In cases where Pythonic programming practices are applicable, we aim to incorporate them to enhance readability and maintain consistency across different languages.


## Macro and Constants

Macros and constants are defined in the header file with ALLCAPS style.


## Structs

Structs are defined in the header file with PascalCase style. A good way to think of structs is that they are the C version of classes, with only attributes and no methods. The methods are defined as global functions that takes these structs as arguments, similar to how Python class methods take the class instance (`self`) as the first argument.


## Functions

C functions uses the `snake_case` style.


## Device Drivers

The device drivers are implemented in the `driver` directory. Each driver is implemented as a separate file, and the devices are grouped by the "manufacturer" or "organization" name, similar to the organization in the Chipyard project. 

The function names are in the format of `<device>_<function>(device_memory_handler, ...)`. For example, the function to initialize a GPIO pin is `gpio_init`.

```c
void gpio_init(GPIO_Type *GPIOx, GPIO_InitType *GPIO_init, GPIO_Pin pin);
```

The `<device>` is added to the beginning of the function name to indicate the device that the function is operating on, and to prevent name conflicts between different devices.

> NOTE:
> Current code only supports using device from a single vendor. This might be changed in the future.
> A possible solution is to use `<vendor>_<device>_<function>` to indicate the function is operating on `<device>` from `<vendor>`.


Functions that have very simple effect, like operating on a register, are designed to be inline functions to reduce the overhead of function call. Such functions are declared and defined in the header file together, for example:

```c
static inline void uart_enable_rx_interrupt(UART_Type *UARTx, uint16_t fifo_level) {
  CLEAR_BITS(UARTx->RXCTRL, UART_RXCTRL_RXCNT_MSK);
  SET_BITS(UARTx->RXCTRL, fifo_level << UART_RXCTRL_RXCNT_POS);
  SET_BITS(UARTx->IE, UART_IE_RXWM_MSK);
}
```

