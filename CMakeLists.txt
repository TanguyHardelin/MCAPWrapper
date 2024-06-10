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
include_directories(${MCAP_WRAPPER_PATH}/include)
include_directories(${MCAP_WRAPPER_PATH}/third_parties/)
include_directories(${MCAP_WRAPPER_PATH}/third_parties/mcap)
include_directories(${MCAP_WRAPPER_PATH}/third_parties/foxglove_websocket)

# Set the library output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Find source file:
FILE(GLOB_RECURSE SOURCE_FILE "${MCAP_WRAPPER_PATH}/src/*.c*")

# Find MCAP sources files:
FILE(GLOB_RECURSE MCAP_SOURCE_FILE "${MCAP_WRAPPER_PATH}/third_parties/mcap/*.c*")

# Find WebSocket sources files:
FILE(GLOB_RECURSE FOXGLOVE_WEBSOCKET_SOURCE_FILE "${MCAP_WRAPPER_PATH}/third_parties/foxglove_websocket/*.c*")

# Add library:
project(mcap_wrapper)
add_library(mcap_wrapper SHARED ${SOURCE_FILE} ${MCAP_SOURCE_FILE} ${FOXGLOVE_WEBSOCKET_SOURCE_FILE})
target_link_libraries(mcap_wrapper lz4 zstd Eigen3::Eigen ${OpenCV_LIBRARIES} ssl)

# Install instruction:
set(MCAP_WRAPPER_PUBLIC_HEADER "${MCAP_WRAPPER_PATH}/include/MCAPWriter.h" "${MCAP_WRAPPER_PATH}/include/MCAPReader.h")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${MCAP_WRAPPER_PUBLIC_HEADER}")

install(TARGETS mcap_wrapper 
        LIBRARY 
            DESTINATION /usr/local/mcap_wrapper/lib)
install(FILES include/MCAPWriter.h include/MCAPReader.h include/define.h DESTINATION /usr/local/mcap_wrapper/includes)
install(FILES cmake/MCAPWrapperConfig.cmake DESTINATION /usr/local/mcap_wrapper/cmake)
install(FILES cmake/MCAPWrapperConfig.cmake DESTINATION /usr/lib/cmake/MCAPWrapper)