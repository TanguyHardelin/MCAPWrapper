#!/bin/bash

export OpenCV_DIR=/usr/local/share/OpenCV

# Create mcap_wrapper directories:
sudo rm -rf /usr/local/mcap_wrapper
sudo mkdir /usr/local/mcap_wrapper
sudo mkdir /usr/local/mcap_wrapper/includes
sudo mkdir /usr/local/mcap_wrapper/lib
sudo mkdir /usr/local/mcap_wrapper/cmake

# Copy Header files:
sudo cp ./*.h* /usr/local/mcap_wrapper/includes

# Copy libs:
sudo cp ./*.so* /usr/local/mcap_wrapper/lib
sudo cp ./*.a* /usr/local/mcap_wrapper/lib

# Copy CMAKE
sudo cp ./*.cmake /usr/lib/cmake
sudo cp ./*.cmake /usr/local/mcap_wrapper/cmake