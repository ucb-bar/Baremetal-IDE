########################################################################################################################
# Saturn vector benchmarks (generators/saturn/benchmarks) as Baremetal-IDE targets for the penguin26 chip.
#
# Sources are referenced IN PLACE through SATURN_BENCH_DIR (nothing is copied). Each benchmark has its
# own subdirectory here whose CMakeLists.txt calls penguin26_saturn_bmark(<name>), which mirrors the
# compile rule in benchmarks/Makefile (compile_template / compile_cpp_template):
#   sources  : <bench>/*.c + <bench>/*.S (C) or <bench>/*.cc + <bench>/*.S (C++),
#              plus common/ara/*.c for C benchmarks (the Makefile's COMMON_SRCS minus crt.S/syscalls.c)
#   includes : env/, common/, every benchmark dir (the Makefile puts all of them on the path),
#              Baremetal-IDE root + driver/metal + driver/rocket-chip/rocketcore, utasks/ for C++
#   flags    : -DPREALLOCATE=1 -O2 -g -ffast-math -fno-common -fno-builtin-printf
#              -fno-tree-loop-distribute-patterns, -std=gnu99 (C) / -std=c++17 (C++)
# saturn's common/crt.S and common/syscalls.c are NOT used: glossy provides startup/exit/stdio, and
# common/saturn_compat.c provides setStats/printstr/printhex and syscalls.c's
# "any trap -> exit(1337)" behavior (glossy's default exception handlers spin forever).
#
# Needs RVV_TYPE=2 (rv64gcv_zfh_zvfh, the Makefile's -march):
#   make build CHIP=penguin26 PLATFORM=SIMS RVV_TYPE=2 TARGET=vec-sgemm        # one benchmark
#   make build CHIP=penguin26 PLATFORM=SIMS RVV_TYPE=2 TARGET=saturn-bmarks    # all of them
########################################################################################################################

if (NOT CHIP STREQUAL "penguin26")
  return()
endif()

set(SATURN_BENCH_DIR "${CMAKE_SOURCE_DIR}/../../generators/saturn/benchmarks"
    CACHE PATH "generators/saturn/benchmarks directory (sources are used in place)")
get_filename_component(_penguin26_bm_dir "${SATURN_BENCH_DIR}" ABSOLUTE)

if (NOT ARCH STREQUAL "rv64gcv_zfh_zvfh")
  message(STATUS "penguin26-bmarks: ARCH=${ARCH}; Saturn benchmarks need RVV_TYPE=2 (rv64gcv_zfh_zvfh), skipped")
  return()
endif()
if (NOT EXISTS "${_penguin26_bm_dir}/common/util.h")
  message(WARNING "penguin26-bmarks: SATURN_BENCH_DIR=${_penguin26_bm_dir} has no common/util.h; skipped")
  return()
endif()

# Every benchmark directory in the order of the Makefile's `bmarks` list (its `incs` puts all of
# them on the include path, so headers may resolve across benchmarks).
set(PENGUIN26_BMARKS_C_LIST
  vec-conditional vec-conjugate-gradient vec-conv-3 vec-cos vec-div-approx vec-dotprod vec-dropout
  vec-exp vec-fconv2d vec-fconv3d vec-fdotprod vec-fft vec-iconv2d vec-igemm vec-jacobi2d vec-log
  vec-mixed_width_mask vec-pathfinder vec-roi-align vec-sep-conv-3 vec-sgemm vec-sgemm-v2 vec-sgemm-v3
  vec-sgemv vec-slide-conv vec-softmax vec-strlen vec-spmv vec-square-root-approx vec-transpose-load
  vec-transpose-store vec-mx-axpy vec-mx-bias-add vec-mx-binary vec-mx-binary-large vec-mx-conv1d
  vec-mx-dotprod vec-mx-fma vec-mx-fmacc vec-mx-gemv vec-mx-l2norm vec-mx-masked-binary
  vec-mx-max-reduction vec-mx-reduction-sum vec-mx-rmsnorm vec-mx-scale vec-mx-unary vec-mx-matmul
  vec-optest vec-bdot)
set(PENGUIN26_BMARKS_CXX_LIST vec-tasks vec-daxpy)

set(_penguin26_bm_incs "${_penguin26_bm_dir}/env" "${_penguin26_bm_dir}/common")
foreach(_b IN LISTS PENGUIN26_BMARKS_C_LIST)
  list(APPEND _penguin26_bm_incs "${_penguin26_bm_dir}/${_b}")
endforeach()
# The Makefile also adds Baremetal-IDE itself: 36 benchmarks include
# "driver/rocket-chip/l_trace_encoder/l_trace_encoder.h" relative to its root.
list(APPEND _penguin26_bm_incs "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/driver/metal"
                           "${CMAKE_SOURCE_DIR}/driver/rocket-chip/rocketcore")

add_library(penguin26-saturn-compat STATIC ${CMAKE_CURRENT_SOURCE_DIR}/common/saturn_compat.c)
target_include_directories(penguin26-saturn-compat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)
target_compile_options(penguin26-saturn-compat PRIVATE -O2)

set(_penguin26_bm_flags -DPREALLOCATE=1 -O2 -g -ffast-math -fno-common -fno-builtin-printf
                    -fno-tree-loop-distribute-patterns)

add_custom_target(saturn-bmarks)

# penguin26_saturn_bmark(<name>): one target named after the benchmark directory.
function(penguin26_saturn_bmark name)
  set(_src_dir "${_penguin26_bm_dir}/${name}")
  if (NOT IS_DIRECTORY "${_src_dir}")
    message(WARNING "penguin26-bmarks: ${_src_dir} does not exist; target ${name} skipped")
    return()
  endif()

  if (name IN_LIST PENGUIN26_BMARKS_CXX_LIST)
    file(GLOB _srcs "${_src_dir}/*.cc" "${_src_dir}/*.S")
    add_executable(${name} ${_srcs})
    target_include_directories(${name} PRIVATE ${_penguin26_bm_incs} "${_penguin26_bm_dir}/utasks")
    target_compile_options(${name} PRIVATE ${_penguin26_bm_flags} $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
  else()
    file(GLOB _srcs "${_src_dir}/*.c" "${_src_dir}/*.S")
    file(GLOB _ara  "${_penguin26_bm_dir}/common/ara/*.c")
    add_executable(${name} ${_srcs} ${_ara})
    target_include_directories(${name} PRIVATE ${_penguin26_bm_incs})
    target_compile_options(${name} PRIVATE ${_penguin26_bm_flags} $<$<COMPILE_LANGUAGE:C>:-std=gnu99>)
  endif()

  target_link_libraries(${name} PRIVATE
    -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  )
  # whole-archive so saturn_compat's strong trap_handler always replaces glossy's weak one,
  # even in benchmarks that never call setStats.
  target_link_libraries(${name} PRIVATE
    -Wl,--whole-archive penguin26-saturn-compat -Wl,--no-whole-archive
  )
  target_link_libraries(${name} PRIVATE chip-config rocketcore m)
  add_dependencies(saturn-bmarks ${name})
endfunction()

foreach(_b IN LISTS PENGUIN26_BMARKS_C_LIST PENGUIN26_BMARKS_CXX_LIST)
  add_subdirectory(${_b})
endforeach()

# Not ported: `tacit-hello-world` is in the Makefile's bmarks list but has no source directory in
# this checkout (the original flow links only crt.S + syscalls.c, whose weak main() returns -1).
