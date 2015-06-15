#!/usr/bin/env bash

test -d build || mkdir -p build
cd build && cmake ..
make && valgrind ./main_test
