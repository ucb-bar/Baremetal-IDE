# Common Programming Issues



## Stack and Heap Size Error

Error message:

```bash
relocation truncated to fit: R_RISCV_GPREL_I in .stack
```

Solution:&#x20;

increase stack size or heap size defined in linker.





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



