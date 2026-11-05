########################################################################################################################
# file:  CMakeLists.txt
#
# usage: 
#   Edit "VARIABLES"-section to suit project requirements.
#   Build instructions:
#     cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake
#     cmake --build ./build/ --target all
#
#   Building on Windows:
#     cmake -S ./ -B ./build/ -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake
#     cmake --build ./build/ --target all
#   Cleaning:
#     cmake --build ./build/ --target clean
#   Install:
#     cmake --build ./build/ --target install
########################################################################################################################
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
project(baremetal-ide LANGUAGES ASM C CXX)


#################################
# Options
#################################

option(BAREMETAL_BUILD_RISCV        "Build for RISC-V platform"               ON  )
option(BAREMETAL_BUILD_X86          "Build for x86 platform"                  OFF )

option(CHIP                         "Build for a specific platform"           OFF )
set(LINKER                         "" CACHE STRING "Linker script variant (e.g. sims, chip, flash)")

option(PROF_COV                     "Build with profiling and coverage"       OFF )
option(GCNO_ONLY                    "Only build gcno files"                   OFF )
option(USE_PGO                      "Build with profile guided optimization"  OFF )
option(OPT_INFO                     "Build with optimization information"      OFF )

option(USE_FREERTOS                 "Build with FreeRTOS"                     OFF )
# OFF (gemmelos-bringup: ON) so existing platforms keep building for rv64imafd by default
option(ENABLE_RVV                   "Build for RVV targets"                   OFF)
set(RVV_TYPE                        "" CACHE STRING "RVV mode: 0=No RVV, 1=RVV (rv64gcv_zfh), 2=RVV+Zvfh (rv64gcv_zfh_zvfh)")
set_property(CACHE RVV_TYPE PROPERTY STRINGS 0 1 2)

# Specify whether we are trying to run on the physical chip or in sims/spike
set(PLATFORM                        "CHIP" CACHE STRING "Runtime platform: CHIP or SIMS")

# Define the variable with a default empty value
set(AFDO_PATH "" CACHE STRING "Path to autofdo profile data")
#################################
# Toolchain Targets
#################################

set(CMAKE_EXECUTABLE_SUFFIX            ".elf")


#################################
# Flags
#################################

# CPU architecture
# RVV_TYPE takes precedence when provided:
#   0 -> no RVV
#   1 -> existing RVV config
#   2 -> RVV + Zvfh (f16 vector capable toolchain/ISA required)
if (NOT RVV_TYPE STREQUAL "")
  set(_RVV_MODE "${RVV_TYPE}")
else()
  if (ENABLE_RVV)
    set(_RVV_MODE "1")
  else()
    set(_RVV_MODE "0")
  endif()
endif()

if (_RVV_MODE STREQUAL "0")
  set(ARCH "rv64imafd")
elseif (_RVV_MODE STREQUAL "1")
  set(ARCH "rv64gcv_zfh")
elseif (_RVV_MODE STREQUAL "2")
  set(ARCH "rv64gcv_zfh_zvfh")
else()
  message(FATAL_ERROR "Invalid RVV_TYPE='${_RVV_MODE}'. Use 0, 1, or 2.")
endif()

message(STATUS "RVV mode=${_RVV_MODE} ARCH=${ARCH}")
set(ABI                 "lp64d")
set(CMODEL              "medany")
set(ARCH_FLAGS          -march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL})

# spec
set(SPECS               "nosys.specs")
set(SPEC_FLAGS          -specs=${SPECS})


# convert CHIP to lowercase
string(TOLOWER ${CHIP} CHIP)
string(TOLOWER "${PLATFORM}" PLATFORM_LOWER)

# Resolve linker variant:
# - If LINKER is explicitly provided, honor it.
# - Otherwise derive from PLATFORM to avoid accidental "-off.ld".
if ((NOT DEFINED LINKER) OR (LINKER STREQUAL "") OR (LINKER STREQUAL "OFF") OR (LINKER STREQUAL "off"))
  if (PLATFORM_LOWER STREQUAL "sims")
    set(LINKER "sims")
  else()
    set(LINKER "chip")
  endif()
endif()
string(TOLOWER "${LINKER}" LINKER)
message(STATUS "Resolved linker variant LINKER=${LINKER} (PLATFORM=${PLATFORM})")

# linker script
# HACK: ideally this should be handled by glossy, but currently i couldn't 
# figure out a way to propagate the LINKER_SCRIPT variable to the compile and link commands
# CHIP unset and CHIP=default are the same thing: the generic platform in platform/default,
# linked with glossy.ld. (platform/default has no default.ld of its own, so an explicit
# -DCHIP=default must NOT fall into the per-chip branch below.)
if ((NOT CHIP) OR (CHIP STREQUAL "default"))
  message(STATUS "Chip not specified, using default configuration")
  set(CHIP "default")

  set(LINKER_SCRIPT       "${CMAKE_SOURCE_DIR}/glossy/glossy.ld")
else()
  set(LINKER_SCRIPT       "${CMAKE_SOURCE_DIR}/platform/${CHIP}/${CHIP}.ld")
  # Honor a linker variant when the file exists: LINKER=<v> -> platform/<chip>/<chip>-<v>.ld
  # Falls back to the default <chip>.ld otherwise, so existing builds are unchanged.
  if (EXISTS "${CMAKE_SOURCE_DIR}/platform/${CHIP}/${CHIP}-${LINKER}.ld")
    set(LINKER_SCRIPT     "${CMAKE_SOURCE_DIR}/platform/${CHIP}/${CHIP}-${LINKER}.ld")
  endif()
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/platform/${CHIP})

add_compile_options(-O1)
add_compile_options(-Wall -Wextra)
add_compile_options(-Wno-error=coverage-mismatch)

if (PROF_COV)
  message(STATUS "Building with profiling and coverage")
  add_compile_options(-fprofile-arcs -ftest-coverage)
endif()

if (GCNO_ONLY)
  message(STATUS "Building only gcno files")
  add_compile_options(-ftest-coverage)
endif()

if (USE_PGO)
  add_compile_options(-fprofile-use)
endif()

if (OPT_INFO)
  add_compile_options(-fopt-info)
endif()

# Check if the variable is not empty and add compiler options accordingly
if(NOT "${AFDO_PATH}" STREQUAL "")
    message(STATUS "Using AutoFDO profile from: ${AFDO_PATH}")
    add_compile_options(-fauto-profile=${AFDO_PATH})
endif()

# add_compile_options(-ffunction-sections -fdata-sections -fno-common -fno-builtin-printf -fno-pie)
# add_compile_options(-Wall -Wextra -Warray-bounds -Wno-unused-parameter -Wcast-qual)
add_compile_options(${ARCH_FLAGS})
add_compile_options(${SPEC_FLAGS})

add_link_options(-static)
# add_link_options(-ffunction-sections -fdata-sections -fno-common -fno-builtin-printf -fno-pie)
# add_link_options(-Wall -Wextra -Warray-bounds -Wno-unused-parameter -Wcast-qual)
add_link_options(-nostartfiles)
add_link_options(${ARCH_FLAGS})
add_link_options(${SPEC_FLAGS})
add_link_options(-T ${LINKER_SCRIPT})
# only enable this if using the LLVM toolchain

if (PROF_COV)
  add_link_options(-lgcov)
endif()

#################################
# Build
#################################

add_executable(app
  app/src/main.c
)

target_include_directories(app PUBLIC app/include)

#################################
# Dependencies
#################################

add_subdirectory(driver)
add_subdirectory(glossy)
add_subdirectory(lib)
add_subdirectory(examples)

target_link_libraries(app PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

if (PROF_COV)
  target_link_libraries(app PRIVATE gcov)
endif()

#################################
# Build Target Directories
#################################

add_subdirectory(penguin26-tests)
add_subdirectory(penguin26-bmarks)

#################################
# Library Installation
#################################

set(CMAKE_INSTALL_PREFIX "$ENV{RISCV}/riscv64-unknown-elf")

set_property(TARGET glossy PROPERTY PUBLIC_HEADER
  glossy/include/riscv.h
  glossy/include/riscv_encoding.h
  glossy/include/riscv_interrupt.h
)

set_property(TARGET glossy PROPERTY RESOURCE
  glossy/glossy.ld
  glossy/glossy.specs
)

install(TARGETS glossy
  LIBRARY DESTINATION "lib"
  PUBLIC_HEADER DESTINATION "include"
  RESOURCE DESTINATION "lib"
)

install(TARGETS htif
  LIBRARY DESTINATION "lib"
  PUBLIC_HEADER DESTINATION "include"
  RESOURCE DESTINATION "lib"
)
