#!/bin/bash

# Install dependency
sudo apt install libwebsocketpp-dev

# Uodate ldconfig
sudo ldconfig

# Create build directory
mkdir build
cd build

# Build
cmake ..
cmake --build .