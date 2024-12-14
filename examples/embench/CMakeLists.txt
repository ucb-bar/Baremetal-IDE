# Define the list of benchmarks
option (EMBENCH_ENABLE_TRACE_PRINT "Enable trace print for benchmarks" OFF)
option (EMBENCH_ENABLE_TRACE_DMA "Enable trace dma for benchmarks" OFF)

if (EMBENCH_ENABLE_TRACE_PRINT)
    add_definitions(-DUSE_L_TRACE)
    add_definitions(-DUSE_L_TRACE_PRINT)
endif()

if (EMBENCH_ENABLE_TRACE_DMA)
    add_definitions(-DUSE_L_TRACE)
    add_definitions(-DUSE_L_TRACE_DMA)
endif()

set(BENCHMARKS
    dummy
    wikisort
    nettle-sha256
    # Add other benchmarks here
)

# Create the support library
add_library(beebsc STATIC common/src/beebsc.c)
target_include_directories(beebsc PUBLIC common/inc)

# Create executables for each benchmark
foreach(benchmark ${BENCHMARKS})
    # Create the benchmark-specific library
    add_library(${benchmark}_lib STATIC ${benchmark}/lib${benchmark}.c)
    target_include_directories(${benchmark}_lib PUBLIC common/inc)

    # Create the executable
    add_executable(${benchmark} common/src/main.c)
    
    # Link dependencies
    target_link_libraries(${benchmark} 
        PUBLIC ${benchmark}_lib
        PUBLIC beebsc
        PUBLIC l_trace_encoder
        PRIVATE -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive -lm
    )
    if (PROF_COV)
        target_link_libraries(${benchmark} PRIVATE gcov)
    endif()
    target_link_libraries(${benchmark} PRIVATE lbr)
endforeach()
