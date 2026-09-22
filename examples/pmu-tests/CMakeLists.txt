set (TESTS
  trace-hello
  pmu-test-load
  pmu-test-store
  pmu-test-inhibit
  ltrace-dma-basic-test
  ltrace-dma-branch-predict-test
  ltrace-dma-branch-target-test
  ltrace-dma-overflow-test
  ltrace-dma-print-test
  ltrace-dma-ring-test
  ltrace-dma-stop-test
  ltrace-dma-workload-test
  ltrace-dma-test
  ltrace-hpm-test
  ltrace-sync-test
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
