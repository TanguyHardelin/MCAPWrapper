# FindMCAPWrapper.cmake for mcap_wrapper

# Set CMAKE specific variables
set(MCAPWrapper_FOUND TRUE)
set(PACKAGE_FIND_NAME "MCAP_WRAPPER")
set(PACKAGE_FIND_VERSION "1")
set(PACKAGE_FIND_VERSION_MAJOR "1")
set(PACKAGE_FIND_VERSION_MINOR "0")
set(PACKAGE_VERSION "1")

# Define the minimum required version of CMake
cmake_minimum_required(VERSION 3.0)

# Define the module name
set(MCAPWRAPPER_MODULE_NAME mcap_wrapper)

# Define the module version
set(MCAPWRAPPER_VERSION 1.0)

# Find library dependencies:
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Define the module paths
set(MCAPWRAPPER_INCLUDE_DIR "/usr/local/mcap_wrapper/includes")
set(MCAPWRAPPER_LIBRARY_DIR "/usr/local/mcap_wrapper/lib")
set(MCAPWRAPPER_LIBRARIES "/usr/local/mcap_wrapper/lib/libmcap_wrapper.so" lz4 zstd Eigen3::Eigen ${OpenCV_LIBRARIES} ssl)


