cmake_minimum_required(VERSION 3.18)

# Set C++ 17 (for mcap)
set(CMAKE_CXX_STANDARD 17)


# Dependencies:
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

# `MCAP_WRAPPER_PATH` is used for locating sources
if(NOT MCAP_WRAPPER_PATH)
    set(MCAP_WRAPPER_PATH ${CMAKE_SOURCE_DIR})
endif()

# Include sub directory:
include_directories(${MCAP_WRAPPER_PATH}/third_parties/)
include_directories(${MCAP_WRAPPER_PATH}/third_parties/mcap)
include_directories(${MCAP_WRAPPER_PATH}/include)

# Find source file:
FILE(GLOB_RECURSE SOURCE_FILE "${MCAP_WRAPPER_PATH}/src/*.c*")

# Find MCAP sources files:
FILE(GLOB_RECURSE MCAP_SOURCE_FILE "${MCAP_WRAPPER_PATH}/third_parties/mcap/*.c*")


# Add library:
project(mcap_wrapper)
message(MCAP_SOURCE_FILE " ${MCAP_SOURCE_FILE}")
message(SOURCE_FILE " ${SOURCE_FILE}")
add_library(mcap_wrapper SHARED ${SOURCE_FILE} ${MCAP_SOURCE_FILE})
target_link_libraries(mcap_wrapper lz4 zstd Eigen3::Eigen ${OpenCV_LIBRARIES})