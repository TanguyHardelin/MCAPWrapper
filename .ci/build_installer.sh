#!/bin/bash

# Create installation folder 
cd install_dependencies
mkdir tmp_install_directory
cp ../../include/*.h tmp_install_directory
cp ../../cmake/*.cmake tmp_install_directory
cp ../../lib/*.a tmp_install_directory
cp ../../lib/*.so tmp_install_directory
cp mcap_wrapper_install_script.sh tmp_install_directory

# Use makeself for creating installer:
./makeself.sh tmp_install_directory install_mcap_wrapper "Install MCAP Wrapper" ./mcap_wrapper_install_script.sh
chmod +x install_mcap_wrapper

# Clear install
rm -rf tmp_install_directory