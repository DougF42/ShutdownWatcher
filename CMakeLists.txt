cmake_minimum_required (VERSION 3.0)

project (testParser)
set(CMAKE_BUILD_TYPE Debug)
add_executable(testParser testParser.cpp parseconfig.cpp)
#  target_link_libraries(shutdownwatcher wiringPi)

