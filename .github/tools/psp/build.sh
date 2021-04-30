#!/bin/bash

echo PSPDEV = $PSPDEV
echo psp-config = `psp-config --psp-prefix`

cmake -S. -Bbuild_psp -DCMAKE_BUILD_TYPE=$1 -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchains/pspsdk-psp.cmake
cmake --build build_psp -- -j $(nproc)


