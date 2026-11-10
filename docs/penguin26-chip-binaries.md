# Building a penguin26 binary that runs on the chip

A chip binary is a `PLATFORM=CHIP` build. The difference from the `PLATFORM=SIMS` ELFs used in RTL
simulation and the FireSim SIMS runs is how it talks to the outside world:

| | `PLATFORM=SIMS` | `PLATFORM=CHIP` |
|---|---|---|
| Console (`printf`) | HTIF | UART0, 115200 baud, 8N2 |
| UART setup | none | `uart_init()` before `main()` (automatic, see below) |
| End of program | `main()`'s return value through HTIF `tohost` | with `-D PENGUIN26_EXIT_TOHOST=ON`: drain UART0, then `tohost` |
| Loaded by | the simulator | the bringup FPGA's TSI host, over serial-TL |

Always build chip binaries with `PENGUIN26_EXIT_TOHOST=ON`. Without it, the host never sees the
program finish: glossy's own exit writes `tohost` into Shuttle's write-back L1 data cache, and the
host reads DRAM behind it.

## 1. A program inside Baremetal-IDE (recommended)

```bash
make build CHIP=penguin26 PLATFORM=CHIP TARGET=hello_world EXTRA_CMAKE_ARGS="-D PENGUIN26_EXIT_TOHOST=ON"
# or
cmake -S . -B build -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D CHIP=penguin26 -D PLATFORM=CHIP \
      -D PENGUIN26_EXIT_TOHOST=ON
cmake --build build --target hello_world      # -> build/penguin26-tests/hello_world/hello_world.elf
```

For a new test, copy `penguin26-tests/atlas_example/` and follow "Writing a new penguin26 test" in the
README. The same source builds for both platforms: write output with `printf` and return 0 from
`main()` for PASS.

Options (CMake cache variables):

| Option | Default | Meaning |
|---|---|---|
| `PENGUIN26_EXIT_TOHOST` | `OFF` | Report `main()`'s return value to the TSI host. Use `ON` for the chip. Not allowed with `PLATFORM=SIMS` |
| `PENGUIN26_UART_BAUDRATE` | `115200` | UART0 baud. Keep 115200 for the chip and the tethered FireSim images |
| `RVV_TYPE` | (RVV off) | `1`: `rv64gcv_zfh` (Saturn vector unit), `2`: adds `zvfh` |
| `LINKER` | | Use `platform/penguin26/penguin26-<LINKER>.ld` instead of `penguin26.ld` |

## 2. A program built by your own build system (compiler deliveries, model kits)

Reproduce what the Baremetal-IDE CMake does for `CHIP=penguin26 PLATFORM=CHIP`:

- **Toolchain:** `riscv64-unknown-elf-gcc` (GCC 13.2.0 is the one the kits were built and checked with).
- **Compile:**
  - `-march=rv64gcv_zfh -mabi=lp64d -mcmodel=medany -specs=nosys.specs`
  - `-D TERMINAL_DEVICE_UART0 -D GLOSSY_NO_LBR_CSR -D PENGUIN26_EXIT_TOHOST -D PENGUIN26_UART_BAUDRATE=115200`

  Do **not** define `TERMINAL_DEVICE_HTIF`, which is the SIMS console.
- **Board support:** compile and link
  - `glossy/` (startup `src/startup/crt0.S` and `init.S`, plus syscalls);
  - `platform/penguin26/` (`chip.c` and `src/*.c`, including `src/uart_stdio.c`);
  - the driver libraries `chip-config` links (`rocketcore`, `clint`, `plic`, `htif`, `uart`,
    `l_trace_encoder`; see the end of `platform/penguin26/CMakeLists.txt`).

  The simplest way is to build the `chip-config` library from this tree with the CMake above and link
  the static libraries it produces.
- **Link:** `-static -nostartfiles -Wl,--wrap=main -T <linker script>`, with the same `-march/-mabi/-mcmodel`.
  - `--wrap=main` is what makes crt0 call `__wrap_main()` (`uart_stdio.c`), which initializes UART0,
    runs your `main()` and then does the tohost exit.
  - Without it, `printf` prints nothing and the host never sees the end of the run.
- **Memory:**
  - DRAM starts at `0x80000000`.
  - `platform/penguin26/penguin26.ld` keeps code, data, heap and stack below `0x90000000` (256 MiB),
    leaving `0x90000000` and up for Atlas data.
  - Programs with large images (e.g. TinyLlama's `tinyllama.ld`, `.rodata` at `0x90800000`) can use
    their own linker script. They must keep `ENTRY(_start)`, glossy's sections (`.htif`, `.tdata`/`.tbss`,
    stack and heap symbols) and the same DRAM base.

### Converting an existing SIMS build (the model kits)

The software kits' TinyLlama / SmolVLA / ResNet50 ELFs are SIMS builds. To make chip versions:

1. Swap the console define: `-D TERMINAL_DEVICE_HTIF` becomes
   `-D TERMINAL_DEVICE_UART0 -D PENGUIN26_EXIT_TOHOST -D PENGUIN26_UART_BAUDRATE=115200`.
2. Compile `platform/penguin26/src/uart_stdio.c` into the platform library.
3. Add `-Wl,--wrap=main` to the link.

Fast-mode builds bind their Atlas programs to the linked addresses of their weights. Any change to the
host code or board support (including this one) can move them, so rebind or check after relinking:
- ResNet50's `tools/resnet_sims/fast.py` rebinds automatically (`build.py --platform chip`).
- For TinyLlama / SmolVLA fast, check that every weight symbol from `.rodata` / `.smol_assets` onward
  kept its address against the SIMS ELF.

Keep console output small on the chip. At 115200 baud each character takes about 87 µs, so per-launch
log lines (e.g. TinyLlama's, `-D TINYLLAMA_QUIET=1` turns them off) can dominate wall time. The kits'
per-launch cycle counts are taken around the device work and exclude printing.

## 3. Check before silicon

The tethered FireSim F2 image `penguin26_tethered_frfcfs_f2` runs the chip exactly as wired on the
board: no memory port, everything over 32-bit serial-TL to an emulated bringup FPGA
(`EE290BringupHostConfig`, 16 GB DRAM, UART TSI). It accepts these `PLATFORM=CHIP` +
`PENGUIN26_EXIT_TOHOST=ON` ELFs unchanged.

The software kit's ResNet50 v2 CHIP ELFs at 115200 (default and fast), which exit this way, passed on
it and on its PMU version `penguin26_pmu_tethered_frfcfs_f2`. A CHIP
ELF built without `PENGUIN26_EXIT_TOHOST` prints its output but runs until the simulation's cycle limit.
