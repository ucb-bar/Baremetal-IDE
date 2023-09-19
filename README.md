# Chipyard Baremetal IDE

## Setup

Follow the tutorial [here](https://ucb-ee290c.github.io/tutorials/software/setting-up-riscv-environment) to set up RISC-V development environment.

## Initial Repository Setup

After cloning the repo, we need to initialize the submodules by executing

```bash
git submodule init
git submodule update
```

#### Compile for simulation

```bash
make build USE_HTIF=1
```

#### Generate binary

```bash
make bin
```

#### Debug the SoC

Terminal 1

```bash
openocd.exe -f ./examplechip.cfg
```

Terminal 2

```bash
riscv64-unknown-elf-gdb.exe --eval-command="target extended-remote localhost:3333"
```
