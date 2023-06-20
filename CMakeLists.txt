cmake_minimum_required (VERSION 3.0)

project (shutdownwatcher)
set(CMAKE_BUILD_TYPE Debug)
add_executable(shutdownwatcher shutdownwatcher.cpp parseDefaults.cpp Parser.cpp EditConfig.cpp)
target_link_libraries(shutdownwatcher wiringPi)
