#!/bin/bash -e
# $File: run.sh
# $Date: Wed Feb 15 23:13:39 2017 +0800
# $Author: jiakai <jia.kai66@gmail.com>

cd $(dirname $0)

MAKE="make -f Makefile.impl"

if [ "$1" = "--clean" ]; then
    rm -rf build main*
    echo "clean finished"
    exit 0
fi

$MAKE TARGET=main_prof CXXFLAGS=-DMIDOUT_PROFILING BUILD_DIR=build/prof

rm -rf output
mkdir -p output
cd output
ln -sv ../main_prof main

if [ "$1" = "-i" ]; then
    cat <<EOF
--------------------------------------------------------------------------------
You would be dropped into a shell now; you can run ./main with some opr mode,
and unused modes would be stripped in the next build.
EOF
    bash
else
    ./main 1 1 p
fi

cd ..

./gen_header.py -o output/midout_gen.h output/midout_trace.*

$MAKE V= TARGET=main_midout \
    CXXFLAGS='-include output/midout_gen.h' BUILD_DIR=build/run

$MAKE TARGET=main_vanilla BUILD_DIR=build/vanilla

function print_stat {
    echo "$1: $(stat -c '%s' $2)"
    nm $2 | c++filt | grep calc | sed -e 's/^/  /'
}


echo "-----------------"
print_stat vanilla main_vanilla
print_stat midout main_midout
