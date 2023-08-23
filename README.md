# Chipyard Baremetal IDE

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
