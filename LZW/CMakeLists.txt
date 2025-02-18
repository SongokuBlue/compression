cmake_minimum_required(VERSION 3.22)
project(LZW)

set(CMAKE_CXX_STANDARD 20)

add_executable(LZW main.cpp GIF.cpp GIF.h Text.h Text.cpp Video.h Video.cpp)
