cmake_minimum_required(VERSION 3.18)

# Include OpenCV library
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Include `mcap_wrapper` library
find_package(MCAPWrapper REQUIRED)
include_directories(${MCAPWRAPPER_INCLUDE_DIR}) # Headers
include_directories(${CMAKE_SOURCE_DIR}/include)

project(UNIT_TEST)
add_executable(UNIT_TEST ${CMAKE_SOURCE_DIR}/src/main.cpp)
# Signal ressource folder
target_compile_definitions(UNIT_TEST PRIVATE RESSOURCE_PATH="${CMAKE_SOURCE_DIR}/ressources")
target_link_libraries(UNIT_TEST ${MCAPWRAPPER_LIBRARIES} ${OpenCV_LIBRARIES} Eigen3::Eigen pthread)