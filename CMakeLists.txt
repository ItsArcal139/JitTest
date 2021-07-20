cmake_minimum_required(VERSION 3.20)
project(JitTest)

set(CMAKE_CXX_STANDARD 23)

add_executable(JitTest main.cpp Logger.cpp Logger.h Text.cpp Text.h)
