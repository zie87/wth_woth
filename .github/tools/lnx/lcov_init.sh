#/bin/bash

build_dir="${1}"
dest_dir="${2}"


echo $build_dir
echo $dest_dir

mkdir -p ${dest_dir}

lcov --zerocounters --directory ${build_dir}
lcov --capture --initial --directory ${build_dir} --output-file ${dest_dir}/cov_base.info

