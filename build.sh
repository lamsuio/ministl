#!/bin/env bash

test -d build && rm -fr build/* || mkdir -p build
cd build && cmake ..

