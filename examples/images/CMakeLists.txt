cmake_minimum_required(VERSION 3.18)

# Include libraries
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(MCAPWrapper REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${MCAPWRAPPER_INCLUDE_DIR}) # Headers

project(SimpleImageWriter)
add_executable(SimpleImageWriter ${CMAKE_SOURCE_DIR}/src/main.cpp)
# Signal ressource folder
target_compile_definitions(SimpleImageWriter PRIVATE RESSOURCE_PATH="${CMAKE_SOURCE_DIR}/ressources")
target_link_libraries(SimpleImageWriter ${MCAPWRAPPER_LIBRARIES} ${OpenCV_LIBRARIES})