cmake_minimum_required(VERSION 3.18)

# Include OpenCV library
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(MCAPWrapper REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${MCAPWRAPPER_INCLUDE_DIR}) # Headers

project(SimplePositionWriter)
add_executable(SimplePositionWriter ${CMAKE_SOURCE_DIR}/src/main.cpp)
# Signal ressource folder
target_compile_definitions(SimplePositionWriter PRIVATE RESSOURCE_PATH="${CMAKE_SOURCE_DIR}/ressources")
target_link_libraries(SimplePositionWriter ${MCAPWRAPPER_LIBRARIES} ${OpenCV_LIBRARIES})