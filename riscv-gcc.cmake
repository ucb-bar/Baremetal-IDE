#################################
# RISCV Toolchain
#################################
option(RISCV                "Build for RISC-V"                  ON)

set(CMAKE_SYSTEM_NAME       "Generic" CACHE STRING "")
set(CMAKE_SYSTEM_PROCESSOR  "riscv"   CACHE STRING "")

set(TOOLCHAIN_PREFIX        "riscv64-unknown-elf-")

set(CMAKE_AR                "${TOOLCHAIN_PREFIX}ar")
set(CMAKE_ASM_COMPILER      "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_C_COMPILER        "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER      "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_LINKER            "${TOOLCHAIN_PREFIX}ld")
set(CMAKE_OBJCOPY           "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP           "${TOOLCHAIN_PREFIX}objdump")
set(CMAKE_SIZE              "${TOOLCHAIN_PREFIX}size")

set(CMAKE_FIND_ROOT_PATH    "${TOOLCHAIN_PATH}/riscv64-unknown-elf/")
set(CMAKE_INCLUDE_PATH      "${TOOLCHAIN_PATH}/riscv64-unknown-elf/include/")
set(CMAKE_LIBRARY_PATH      "${TOOLCHAIN_PATH}/riscv64-unknown-elf/lib/")
set(CMAKE_PROGRAM_PATH      "${TOOLCHAIN_PATH}/riscv64-unknown-elf/bin/")


set(CMAKE_CROSSCOMPILING TRUE)

# prevent the toolchain from searching for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

