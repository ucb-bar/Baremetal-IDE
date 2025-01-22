set (TESTS
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
  PUBLIC mada_timer
  PRIVATE -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
  PUBLIC m
  )
endforeach()
