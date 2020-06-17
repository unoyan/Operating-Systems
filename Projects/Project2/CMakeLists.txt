# cmake_minimum_required(VERSION <specify CMake version here>)
project(P2_Thread CPP)

set(CMAKE_C_STANDARD 99)

add_executable(P2_Thread bbar.c plane.c pthread_sleep.c main.cpp)