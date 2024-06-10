#!/bin/bash

mkdir build
cd build
cmake ..
make -j
./UNIT_TEST