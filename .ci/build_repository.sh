#!/bin/bash

# Install dependency
sudo apt update
sudo apt install build-essential
sudo apt install cmake
sudo apt install libssl-dev      
sudo apt install libwebsocketpp-dev


# Create build directory
mkdir build
cd build

# Build
cmake ..
cmake --build .