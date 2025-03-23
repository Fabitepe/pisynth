cmake_minimum_required(VERSION 3.10)
project(pisynth)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_BUILD_TYPE Debug)

file(GLOB SOURCES "src/*.c*")

add_executable(pisynth ${SOURCES})

include_directories(include /usr/local/include/rtmidi/)

target_link_libraries(pisynth -ldl -lpthread -lm -latomic -lrtmidi -lasound)