#!/usr/bin/env bash

test -d build || mkdir -p build
cd build && cmake ..
make && valgrind --leak-check=full --show-leak-kinds=all -v ./main_test
