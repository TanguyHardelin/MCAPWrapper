#!/bin/bash

# Install dependency
sudo apt update
sudo apt upgrade
sudo apt install build-essential
sudo apt install cmake
sudo apt install libssl-dev      
sudo apt install libopencv-dev   
sudo apt install libwebsocketpp-dev

# Export environnement variables:
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/local/lib/pkgconfig
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/local/lib

# Uodate ldconfig
sudo ldconfig

# Create build directory
mkdir build
cd build

# Build
cmake ..
cmake --build .