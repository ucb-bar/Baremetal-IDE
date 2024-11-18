# must be run from the root directory
# generate the baseline
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=OFF -D USE_PGO=OFF
cmake --build ./build/ --target nettle-sha256
spike build/examples/embench/nettle-sha256.elf > build/test.txt
# generate the coverage profile
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=ON -D USE_PGO=OFF
cmake --build ./build/ --target nettle-sha256 
spike build/examples/embench/nettle-sha256.elf > build/test_cov.txt
# write the coverage files
python3 /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/gcov/dump_gcda.py /scratch/iansseijelly/chipyard/software/baremetal-ide/build/test_cov.txt 
# perform PGO
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=OFF -D USE_PGO=ON
cmake --build ./build/ --target nettle-sha256
spike build/examples/embench/nettle-sha256.elf > build/test_after.txt
# diff
diff build/test.txt build/test_after.txt