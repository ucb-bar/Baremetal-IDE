.PHONY: build
build:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=dsp24
	cmake --build ./build/ --target app

.PHONY: clean
clean:
	rm -rf build

.PHONY: dump
dump:
	riscv64-unknown-elf-objdump -D  build/app.elf > dump.txt