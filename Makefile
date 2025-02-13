PREFIX = riscv64-elf-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
DG = $(PREFIX)gdb
SIZE = $(PREFIX)size

# Can be used to change HART for gdb debugging
PORT = 3333

PREFIX = riscv64-elf-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
DG = $(PREFIX)gdb
SIZE = $(PREFIX)size

PORT = 3333

.PHONY: build
dsp24:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=dsp24
	cmake --build ./build/ --target app

bearly24:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target app

.PHONY: borai
borai:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target borai

.PHONY: pend
pend:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target encoders

.PHONY: build
build:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=$(CHIP)
	cmake --build ./build/ --target $(TARGET_NAME)

.PHONY: ocd
ocd:
	openocd -f ./platform/$(CHIP)/$(CHIP).cfg

.PHONY: gdb
gdb:
	$(DG) $(BINARY) --eval-command="target extended-remote localhost:$(PORT)" --eval-command="monitor reset"

.PHONY: clean
clean:
	rm -rf build

.PHONY: checktsi
checktsi:
	uart_tsi +tty=$(TTY) +baudrate=921600 +no_hart0_msip +init_write=0x80001000:0xb0bacafe +init_read=0x80001000 none

.PHONY: dump
dump:
	riscv64-unknown-elf-objdump -D  build/app.elf > dump.txt

.PHONY: tsi
tsi:
	uart_tsi +tty=$(TTY) +baudrate=921600 $(BINARY)
