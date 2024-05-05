cmake_minimum_required(VERSION 3.10)

# Include libraries
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(MCAPWrapper REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${MCAPWRAPPER_INCLUDE_DIR}) # Headers
include_directories(${CMAKE_SOURCE_DIR}/include) # Headers

project(SimpleJSONWriter)
add_executable(SimpleJSONWriter ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(SimpleJSONWriter ${MCAPWRAPPER_LIBRARIES})