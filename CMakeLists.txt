cmake_minimum_required(VERSION 2.6)
project(engineering_units)

add_executable(engineering_units main.cpp)

install(TARGETS engineering_units RUNTIME DESTINATION bin)
