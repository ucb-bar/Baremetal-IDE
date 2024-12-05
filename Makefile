PREFIX = riscv64-unknown-elf-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
DG = $(PREFIX)gdb
SIZE = $(PREFIX)size

# Can be used to change HART for gdb debugging
PORT = 3333

.PHONY: build
dsp24:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=dsp24
	cmake --build ./build/ --target app

bearly24:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target app

# Example: make build TARGET=borai CHIP=bearly24
.PHONY: build
build:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=$(CHIP)
        cmake --build ./build/ --target $(TARGET)

# Example: make ocd CHIP=bearly24
.PHONY: ocd
ocd:
        openocd -f ./platform/$(CHIP)/$(CHIP).cfg

# Example: make gdb BINARY=build/borai/boraiq.elf
.PHONY: gdb
gdb:
        $(DG) $(BINARY) --eval-command="target extended-remote localhost:$(PORT)" --eval-command="monitor reset"

# Example: make dump BINARY=build/borai/boraiq.elf
.PHONY: dump
dump:
        $(OD) -D  $(BINARY) > $(BINARY).dump

.PHONY: clean
clean:
	rm -rf build
