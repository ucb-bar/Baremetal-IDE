########################################################################################################################
# Atlas NPU baremetal tests for the penguin26 chip.
#
# One target `atlas_<name>` per generators/atlas-npu/baremetal/assembly/<name>.S (top level only, the
# same set atlas-npu's Makefile and run_asm_tests.sh use). The test C file is generated at BUILD time
# into ${PENGUIN26_TESTS_ATLAS_GEN_DIR} by scripts/atlas/gen_atlas_tests.py, which imports atlas-npu's
# assembler.py and runs its generators/gen_*.py; the generated harness uses platform/penguin26 hal_atlas.
# Nothing in atlas-npu is copied or edited.
#
#   make build CHIP=penguin26 PLATFORM=SIMS TARGET=atlas_alu_basic \
#     EXTRA_CMAKE_ARGS="-DPENGUIN26_TESTS_ATLAS_PYTHON=/path/to/venv/bin/python"
#   make build CHIP=penguin26 PLATFORM=SIMS TARGET=atlas-tests          # all of them
#
# The golden generators need numpy, torch and numba: point PENGUIN26_TESTS_ATLAS_PYTHON at an interpreter
# that has them.
########################################################################################################################

set(PENGUIN26_TESTS_ATLAS_DIR    "${CMAKE_SOURCE_DIR}/../../generators/atlas-npu/baremetal"
    CACHE PATH "atlas-npu/baremetal directory (assembly/, assembler.py, generators/)")
set(PENGUIN26_TESTS_ATLAS_PYTHON "python3"
    CACHE STRING "Python used to run gen_atlas_tests.py and atlas-npu's gen_*.py (needs numpy/torch/numba)")
set(PENGUIN26_TESTS_ATLAS_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated"
    CACHE PATH "Where the generated atlas_<name>.c (and golden JSON) are written")

get_filename_component(_penguin26_atlas_dir "${PENGUIN26_TESTS_ATLAS_DIR}" ABSOLUTE)
set(_penguin26_atlas_gen_script "${CMAKE_SOURCE_DIR}/scripts/atlas/gen_atlas_tests.py")

if (NOT EXISTS "${_penguin26_atlas_dir}/assembler.py")
  message(WARNING "penguin26-tests: no assembler.py in PENGUIN26_TESTS_ATLAS_DIR=${_penguin26_atlas_dir}; Atlas tests skipped")
  return()
endif()

file(GLOB _penguin26_atlas_asm "${_penguin26_atlas_dir}/assembly/*.S")
list(SORT _penguin26_atlas_asm)

add_custom_target(atlas-tests)
set(_penguin26_atlas_n 0)

foreach(_asm IN LISTS _penguin26_atlas_asm)
  get_filename_component(_name ${_asm} NAME_WE)
  set(_tgt "atlas_${_name}")
  set(_c   "${PENGUIN26_TESTS_ATLAS_GEN_DIR}/atlas_${_name}.c")

  # Generator lookup, same as atlas-npu's Makefile: gen_<name>.py, else mxu[01]_ -> mxu_.
  set(_deps "${_asm}" "${_penguin26_atlas_dir}/assembler.py" "${_penguin26_atlas_gen_script}")
  string(REGEX REPLACE "mxu[01]_" "mxu_" _gen_base "${_name}")
  if (EXISTS "${_penguin26_atlas_dir}/generators/gen_${_name}.py")
    list(APPEND _deps "${_penguin26_atlas_dir}/generators/gen_${_name}.py")
  elseif (EXISTS "${_penguin26_atlas_dir}/generators/gen_${_gen_base}.py")
    list(APPEND _deps "${_penguin26_atlas_dir}/generators/gen_${_gen_base}.py")
  endif()

  add_custom_command(
    OUTPUT  "${_c}"
    COMMAND "${PENGUIN26_TESTS_ATLAS_PYTHON}" "${_penguin26_atlas_gen_script}"
            --atlas-dir "${_penguin26_atlas_dir}"
            --out-dir "${PENGUIN26_TESTS_ATLAS_GEN_DIR}"
            --python "${PENGUIN26_TESTS_ATLAS_PYTHON}"
            "${_name}"
    DEPENDS ${_deps}
    COMMENT "Generating atlas_${_name}.c"
    VERBATIM
  )

  add_executable(${_tgt} "${_c}")

  # Same compile flags as atlas-npu's tests/CMakeLists.txt (-O2 overrides the global -O1).
  target_compile_options(${_tgt} PRIVATE -std=gnu99 -O2 -fno-common -fno-builtin-printf)

  target_link_libraries(${_tgt} PRIVATE
    -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  )
  target_link_libraries(${_tgt} PRIVATE chip-config)
  target_link_libraries(${_tgt} PRIVATE rocketcore)

  add_dependencies(atlas-tests ${_tgt})
  math(EXPR _penguin26_atlas_n "${_penguin26_atlas_n}+1")
endforeach()

message(STATUS "penguin26-tests: ${_penguin26_atlas_n} Atlas test target(s) from ${_penguin26_atlas_dir}/assembly")
