cmake_minimum_required (VERSION 3.0)

project (testParser)
set(CMAKE_BUILD_TYPE Debug)
add_executable(testParser testParser.cpp parse_cfg.cpp)
#  target_link_libraries(shutdownwatcher wiringPi)

