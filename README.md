![](docs/logo_b.png)

# Chipyard Baremetal-IDE

![CI-status](https://img.shields.io/github/actions/workflow/status/ucb-bar/Baremetal-IDE/make-examples.yaml?branch=main&style=flat-square&label=CI&logo=githubactions&logoColor=fff) ![API-Docs-status](https://img.shields.io/github/actions/workflow/status/ucb-bar/Baremetal-IDE/build-docs.yaml?branch=main&style=flat-square&label=Docs&logo=googledocs&logoColor=fff)

> **WARNING⚠️**
> Baremetal-IDE is still under heavy development at the moment, so we don't guarantee the stability and backward-compatibility among versions.

Baremetal-IDE is an all-in-one tool for baremetal-level C/C++ program developments. It is part of the Chipyard ecosystem.

Baremetal-IDE features peripheral configuration, code generation, code compilation, and debugging tools for multiple RISC-V SoCs. With the board support package, user can use either the hardware-abstraction layer (HAL) functions to quickly configure and use the various supported peripheral devices, or can use the low-level (LL) macro definitions to generate code with minimal memory footprint and high performance. The modularity of the framework structure also allows fast integration of new SoCs. 


## Documentation and Getting Started

Please refer to the [Tutorial Website](https://ucb-bar.gitbook.io/chipyard/baremetal-ide/getting-started-with-baremetal-ide) for getting started with Baremetal-IDE, and refer to the [API Docs](https://ucb-bar.github.io/Baremetal-IDE/index.html) for more detailed information on the APIs.


## Simple examples

### Compiling for Spike

```bash
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake
cmake --build ./build/ --target app
```

### Compiling for FE310

```bash

```

### Compiling example programs

```bash

```

