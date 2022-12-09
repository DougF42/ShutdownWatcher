cmake_minimum_required (VERSION 2.6)

project (ShutdownWatcher)
set(CMAKE_BUILD_TYPE Debug)
add_executable(ShutdownWatcher ShutdownWatcher.cpp)
target_link_libraries(ShutdownWatcher wiringPi)

