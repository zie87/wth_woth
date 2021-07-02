#!/bin/bash

set -e

script_dir=$(dirname `readlink -f $0`)
source_dir="${1}"
build_dir="${2}"

cmake -S ${source_dir} -B ${build_dir} \
    -DCMAKE_BUILD_TYPE=Debug \
    -DDISABLE_UI=ON \
    -DENABLE_COVERAGE=ON

cmake --build ${build_dir} -- -j $(nproc)

lcov_build_dir="${build_dir}/lcov"

${script_dir}/lcov_init.sh ${build_dir} ${lcov_build_dir}
cmake --build ${build_dir} --target test
${script_dir}/lcov_gen.sh ${build_dir} ${lcov_build_dir}



