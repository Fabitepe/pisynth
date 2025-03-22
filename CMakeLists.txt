cmake_minimum_required(VERSION 3.10)
project(pisynth)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.c*")

add_executable(pisynth ${SOURCES})

include_directories(include)

target_link_libraries(pisynth -ldl -lpthread -lm -latomic)