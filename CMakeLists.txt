cmake_minimum_required(VERSION 3.10)
project(hw5)

set(CMAKE_CXX_STANDARD 11)

include_directories(ClientServer)

add_executable(hw5
        ClientServer/hw5Client.cc
        ClientServer/hw5Server.cc
        ClientServer/Log.cc
        ClientServer/Log.h
        ClientServer/SafeQueue.cc
        ClientServer/SafeQueue.h
        ClientServer/SetupData.cc
        ClientServer/SetupData.h)
