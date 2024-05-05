cmake_minimum_required(VERSION 3.10)

# Include libraries
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(MCAPWrapper REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${MCAPWRAPPER_INCLUDE_DIR}) # Headers

project(Simple3DWriter)
add_executable(Simple3DWriter ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(Simple3DWriter ${MCAPWRAPPER_LIBRARIES} ${OpenCV_LIBRARIES} )