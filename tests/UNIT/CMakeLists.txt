cmake_minimum_required(VERSION 3.18)

# Include OpenCV library
find_package(OpenCV 4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Include `mcap_wrapper` library
set(MCAP_WRAPPER_PATH ${CMAKE_SOURCE_DIR}/../..)
include(${MCAP_WRAPPER_PATH}/CMakeLists.txt) # Compilation of library
include_directories(${MCAP_WRAPPER_PATH}/include) # Headers

project(UNIT_TEST)
add_executable(UNIT_TEST ${CMAKE_SOURCE_DIR}/src/main.cpp)
# Signal ressource folder
target_compile_definitions(UNIT_TEST PRIVATE RESSOURCE_PATH="${CMAKE_SOURCE_DIR}/ressources")
target_link_libraries(UNIT_TEST mcap_wrapper ${OpenCV_LIBRARIES} pthread)