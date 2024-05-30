#!/bin/bash

# Uodate ldconfig
sudo ldconfig

# Create build directory
mkdir build
cd build

# Build
cmake ..
make -j