#!/bin/bash
set -e

echo PSPDEV = $PSPDEV
echo psp-config = `psp-config --psp-prefix`

cd extern/psp_exception_handler/prx
make clean
make -j $(nproc)
cd -

cmake -S. -Bbuild_psp -DCMAKE_BUILD_TYPE=$1 -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchains/pspsdk-psp.cmake
cmake --build build_psp -- -j $(nproc)
cmake --build build_psp --target package


