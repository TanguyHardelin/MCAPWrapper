#!/bin/bash

cd UNIT
mkdir build
cd build
cmake ..
make -j
./UNIT_TEST