cmake_minimum_required(VERSION 2.6)
project(engineering_units)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(engineering_units INTERFACE)
target_include_directories(engineering_units INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)

install(TARGETS engineering_units RUNTIME DESTINATION bin)
