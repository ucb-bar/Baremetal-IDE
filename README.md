![](docs/logo_b.png)

# Chipyard Baremetal-IDE

![CI-status](https://img.shields.io/github/actions/workflow/status/ucb-bar/Baremetal-IDE/make-examples.yaml?branch=main&style=flat-square&label=CI&logo=githubactions&logoColor=fff) ![API-Docs-status](https://img.shields.io/github/actions/workflow/status/ucb-bar/Baremetal-IDE/build-docs.yaml?branch=main&style=flat-square&label=Docs&logo=googledocs&logoColor=fff)

> **WARNING⚠️**
> Baremetal-IDE is still under heavy development at the moment, so we don't guarantee the stability and backward-compatibility among versions.

Baremetal-IDE is an all-in-one tool for baremetal-level C program developments. It is part of the Chipyard ecosystem.

> **NOTE**
> The scope of Baremetal-IDE is reduced to only support C programming language without C++ support. To use advanced object-oriented programming and other high-level features, please consider the soon coming Rust port, [Baremetal-RS]().

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

### Compiling for penguin26 (EE194/290 tapeout chip)

```bash
make build CHIP=penguin26 PLATFORM=SIMS TARGET=hello_world
make build CHIP=penguin26 PLATFORM=SIMS TARGET=atlas-tests
make build CHIP=penguin26 PLATFORM=SIMS RVV_TYPE=2 TARGET=saturn-bmarks
```

The same with cmake directly: `cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D CHIP=penguin26 -D PLATFORM=SIMS` then `cmake --build ./build/ --target hello_world` (`PLATFORM` defaults to `CHIP`).

`PLATFORM=SIMS` prints over HTIF (Chipyard RTL simulation, `EE290SimConfig`); `PLATFORM=CHIP` prints over UART0 (115200 baud; the platform runs `uart_init` before `main`). The Atlas NPU tests are generated from `generators/atlas-npu` and need a Python with numpy, torch and numba (`EXTRA_CMAKE_ARGS="-DPENGUIN26_TESTS_ATLAS_PYTHON=<python>"`); the Saturn benchmarks are built from `generators/saturn/benchmarks`.

### Writing a new penguin26 test

Copy `penguin26-tests/atlas_example/` (a minimal Atlas NPU test: loads a program into Atlas IMEM with `hal_atlas`, runs it, checks the result, prints PASS/FAIL and returns 0/1), rename the target in its `CMakeLists.txt`, and add the directory to `penguin26-tests/CMakeLists.txt`:

```cmake
add_subdirectory(my_test)
```

Build it, then run it in Chipyard's VCS simulation (`main()` returning 0 passes the simulation):

```bash
make build CHIP=penguin26 PLATFORM=SIMS TARGET=my_test
make -C $chipyard/sims/vcs run-binary CONFIG=EE290SimConfig LOADMEM=1 BINARY=$PWD/build/penguin26-tests/my_test/my_test.elf
```
