#!/bin/bash

echo DEVKITPRO = $DEVKITPRO
echo DEVKITPPC = $DEVKITPPC

cmake -H. -Bbuild_wii -DCMAKE_BUILD_TYPE=$1 -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchains/devkit-wii.cmake
# cmake --build build_wii -- -j $(nproc)
cmake --build build_wii --target wge -- -j $(nproc) 
