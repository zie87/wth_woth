#!/bin/bash

build_dir="${1}"
dest_dir="${2}"

lcov --capture --directory ${build_dir} --output-file ${dest_dir}/cov.info
lcov --add-tracefile ${dest_dir}/cov_base.info --add-tracefile ${dest_dir}/cov.info --output-file ${dest_dir}/cov_total.info
lcov --remove ${dest_dir}/cov_total.info '/usr/include/*' '/usr/lib/*' '*/extern/*' '*/tests/*' '*/playground/*' -o ${dest_dir}/cov_filter.info

genhtml ${dest_dir}/cov_filter.info --output-directory ${dest_dir}/html

