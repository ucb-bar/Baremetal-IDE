#!/bin/bash
set -e
# here should be where the script itself is
cd /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all
HERE=$(pwd)
mkdir -p $HERE/test
cd /scratch/iansseijelly/chipyard/software/baremetal-ide

echo "--- Generating the vanilla baseline ---"
rm -rf build
# generate the vanilla baseline
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=OFF -D USE_PGO=OFF
cmake --build ./build/ --target wikisort 
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/vanilla.log
popd

echo "--- Generating the instrumented coverage profile ---"
rm -rf build 
# generate the instrumented coverage profile
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=ON -D USE_PGO=OFF
cmake --build ./build/ --target wikisort 
spike build/examples/embench/wikisort.elf > build/test_cov.txt
# write the coverage files
python3 /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/gcov/dump_gcda.py /scratch/iansseijelly/chipyard/software/baremetal-ide/build/test_cov.txt 
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/instrumented.log 
popd
# perform PGO
echo "--- Performing PGO ---"
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=OFF -D USE_PGO=ON
cmake --build ./build/ --target wikisort
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/instrumentation_pgo.log
popd

rm -rf build 
# generate a trace
echo "--- Generating tacit trace ---"
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D GCNO_ONLY=ON -D USE_PGO=OFF -D EMBENCH_ENABLE_TRACE_PRINT=ON
cmake --build ./build/ --target wikisort
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/trace.log
popd
pushd /scratch/iansseijelly/ltrace_decoder
cargo run -- --encoded-trace /scratch/iansseijelly/chipyard/sims/vcs/byte_printer.txt --binary /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf --gcno /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/CMakeFiles/wikisort_lib.dir/wikisort/libwikisort.c.gcno --to-gcda --to-afdo
# save the trace afdo file for later use
cp ./trace_afdo.txt $HERE/test/trace_afdo.txt
/scratch/iansseijelly/autofdo/build/create_gcov --binary /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf --profile $HERE/test/trace_afdo.txt --profiler text --gcov_version=2 --gcov trace_afdo_fbdata.afdo
popd

# must not remove build directory, as .gcda files are needed
# perform trace PGO
echo "--- Performing trace PGO ---"
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D PROF_COV=OFF -D USE_PGO=ON
cmake --build ./build/ --target wikisort
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/trace-pgo.log
popd

rm -rf build 
# perform LBR samping
echo "--- Performing LBR sampling ---"
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D EMBENCH_ENABLE_LBR=ON
cmake --build ./build/ --target wikisort 
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/sample-lbr-vanilla.log
popd
# post-process the sampled LBR record
pushd /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all/test 
python3 /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/perf/dump_lbr.py /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all/test/sample-lbr-vanilla.log
/scratch/iansseijelly/autofdo/build/create_gcov --binary /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf --profile /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/perf/lbr_branch.txt --profiler text --gcov_version=2
popd

# perform LBR AutoFDO
echo "--- Performing LBR AutoFDO ---"
rm -rf build 
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D EMBENCH_ENABLE_LBR=OFF -D AFDO_PATH=/scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all/test/fbdata.afdo
cmake --build ./build/ --target wikisort 
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/sample-lbr-afdo.log
popd

# perform trace LBR AutoFDO
echo "--- Performing trace LBR AutoFDO ---"
rm -rf build 
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D EMBENCH_ENABLE_LBR=OFF -D AFDO_PATH=/scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all/test/trace_afdo_fbdata.afdo
cmake --build ./build/ --target wikisort 
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/trace-lbr-afdo.log
popd