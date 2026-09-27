set (TESTS
  trace-hello
  pmu-test-load
  pmu-test-store
  pmu-test-inhibit
  ltrace-dma-test
  ltrace-hpm-test
  speedscope-test
  vpp-test
  sort
)

foreach(test ${TESTS})
  add_executable(${test} src/${test}.c)
  target_link_libraries(${test} 
  PUBLIC l_trace_encoder 
  PRIVATE -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  PUBLIC m
  )
endforeach()

# Native TilePMU MMIO test; distinct from the architectural CSR PMU examples.
if(TILE_PMU_MANIFEST)
  add_executable(tile-pmu-test src/tile-pmu-test.c)
  target_link_libraries(tile-pmu-test PRIVATE
    -Wl,--whole-archive glossy -Wl,--no-whole-archive)
endif()
