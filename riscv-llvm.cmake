#################################
# RISCV Toolchain
#################################
option(RISCV                "Build for RISC-V"                  ON)

set(CMAKE_SYSTEM_NAME       "Generic" CACHE STRING "")
set(CMAKE_SYSTEM_PROCESSOR  "riscv"   CACHE STRING "")

set(TOOLCHAIN_PREFIX        "riscv64-unknown-elf-")

set(MYRISCV                 "$ENV{RISCV}")

set(CMAKE_AR                "${MYRISCV}/bin/llvm-ar")
set(CMAKE_ASM_COMPILER      "${MYRISCV}/bin/clang")
set(CMAKE_C_COMPILER        "${MYRISCV}/bin/clang")
set(CMAKE_CXX_COMPILER      "${MYRISCV}/bin/clang++")
set(CMAKE_LINKER            "${MYRISCV}/bin/lld")
set(CMAKE_RANLIB            "${MYRISCV}/bin/llvm-ranlib")
set(CMAKE_OBJCOPY           "${MYRISCV}/bin/llvm-objcopy")
set(CMAKE_OBJDUMP           "${MYRISCV}/bin/llvm-objdump")
set(CMAKE_SIZE              "${MYRISCV}/bin/llvm-size")

# tell me the cc compiler cmake found
message(STATUS "CC: ${CMAKE_C_COMPILER}")

set(CMAKE_CROSSCOMPILING TRUE)

# prevent the toolchain from searching for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_link_options(-rtlib=libgcc)

# add std lib path
include_directories("$ENV{RISCV}/riscv64-unknown-elf/include")
link_directories("$ENV{RISCV}/riscv64-unknown-elf/lib")
# for libgcc
link_directories("${MYRISCV}/lib/gcc/riscv64-unknown-elf/15.1.0")