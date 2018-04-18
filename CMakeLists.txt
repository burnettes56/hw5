cmake_minimum_required(VERSION 3.10)
project(hw5)

set(CMAKE_CXX_STANDARD 11)

include_directories(hw5Client)
include_directories(hw5Server)

add_executable(hw5
        hw5Client/client.cc
        hw5Client/SafeQueue.cc
        hw5Client/SafeQueue.h)
