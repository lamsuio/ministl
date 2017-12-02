#!/usr/bin/env sh

test -d build || mkdir -p build
cd build && cmake ..
make && valgrind ./main_test
