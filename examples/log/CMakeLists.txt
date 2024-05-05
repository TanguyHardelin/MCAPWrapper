cmake_minimum_required(VERSION 3.10)

# Include JSON library
include_directories(${CMAKE_SOURCE_DIR}/include)

# Include `mcap_wrapper` library
set(MCAP_WRAPPER_PATH ${CMAKE_SOURCE_DIR}/../..)
include(${MCAP_WRAPPER_PATH}/CMakeLists.txt) # Compilation of library

project(SimpleLogWriter)
add_executable(SimpleLogWriter ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(SimpleLogWriter mcap_wrapper)