#!/bin/bash
set -e
# here should be where the script itself is
cd /scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all
HERE=$(pwd)
mkdir -p $HERE/test
cd /scratch/iansseijelly/chipyard/software/baremetal-ide

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
mv ./lbr_branch.txt $HERE/test/lbr_sampling_branch.txt
/scratch/iansseijelly/autofdo/build/create_gcov --binary /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf --profile $HERE/test/lbr_sampling_branch.txt --profiler text --gcov_version=2
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
cargo run -- --encoded-trace /scratch/iansseijelly/chipyard/sims/vcs/byte_printer.txt --binary /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf --gcno /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/CMakeFiles/wikisort_lib.dir/wikisort/libwikisort.c.gcno --to-afdo
# save the trace afdo file for later use
cp ./trace_afdo.txt $HERE/test/trace_afdo.txt
popd
/scratch/iansseijelly/autofdo/build/create_gcov --binary /scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf --profile $HERE/test/trace_afdo.txt --profiler text --gcov_version=2 --gcov $HERE/test/trace_afdo_fbdata.afdo
# perform trace LBR AutoFDO
echo "--- Performing trace LBR AutoFDO ---"
rm -rf build 
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D EMBENCH_ENABLE_LBR=OFF -D AFDO_PATH=/scratch/iansseijelly/chipyard/software/baremetal-ide/scripts/test_all/test/trace_afdo_fbdata.afdo
cmake --build ./build/ --target wikisort 
pushd /scratch/iansseijelly/chipyard/sims/vcs
make run-binary CONFIG=WithLTraceEncoderRocketConfig BINARY=/scratch/iansseijelly/chipyard/software/baremetal-ide/build/examples/embench/wikisort.elf LOADMEM=1
cp /scratch/iansseijelly/chipyard/sims/vcs/output/chipyard.harness.TestHarness.WithLTraceEncoderRocketConfig/wikisort.log $HERE/test/trace-lbr-afdo.log
popd