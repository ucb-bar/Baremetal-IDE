# CMake toolchain definition for RISC-V GCC toolchain
set(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")
set(CMAKE_SYSTEM_PROCESSOR "riscv" CACHE STRING "")

set(TOOLCHAIN_PREFIX   "riscv64-unknown-elf")

set(CMAKE_C_COMPILER   "${TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++")
set(CMAKE_AR           "${TOOLCHAIN_PREFIX}-ar")
set(CMAKE_LINKER       "{TOOLCHAIN_PREFIX}-ld")
set(CMAKE_OBJCOPY      "${TOOLCHAIN_PREFIX}-objcopy")
set(CMAKE_SIZE         "${TOOLCHAIN_PREFIX}-size")
set(CMAKE_STRIP        "${TOOLCHAIN_PREFIX}-ld")
