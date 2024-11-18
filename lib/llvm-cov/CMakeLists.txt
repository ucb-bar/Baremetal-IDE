add_library(llvm-cov STATIC llvm_cov.c)

target_include_directories(llvm-cov PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
