cmake_minimum_required (VERSION 2.6)

project (shutdownwatcher)
set(CMAKE_BUILD_TYPE Debug)
add_executable(shutdownwatcher shutdownwatcher.cpp)
target_link_libraries(shutdownwatcher wiringPi)

