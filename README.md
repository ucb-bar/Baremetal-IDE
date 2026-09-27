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


## Optional native TilePMU platform metadata

Pass three explicit paths to CMake:

- `TILE_PMU_MANIFEST=/path/to/design.pmu.json`
- `TILE_PMU_PLATFORM_JSON=/path/to/design.json`
- `TILE_PMU_SOFTWARE_DIR=/path/to/pmu/software`

The two JSON files must come from the same hardware elaboration. The software
directory contains `pmu_mmio.h`, `tile_pmu.h`, and `generate_platform_header.py`;
in Chipyard it is `generators/pmu/software`. Baremetal-IDE does not assume a
particular checkout layout. Missing inputs are rejected during configuration.

The selected `chip-config` exports a generated `tile_pmu_platform.h` in the build
directory, with the MMIO address, symbolic event IDs, and an X-macro list of IDs,
names, and units. `TILE_PMU_SOURCE` defaults to `boom_tile_0`. This also enables
the `tile-pmu-test` target in `examples/pmu-tests`.

The native MMIO PMU is separate from the existing architectural CSR PMU examples.
See the README in the supplied PMU software directory for accessor usage. In
Chipyard, run `python3 tools/pmu/test-software.py` from the Chipyard root to build
and execute the full-core test. Builds without PMU metadata are unchanged.
