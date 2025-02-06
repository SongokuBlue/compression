cmake_minimum_required(VERSION 3.22)
project(learn)

set(CMAKE_CXX_STANDARD 17)

add_executable(learn main.cpp
        src/huffman.cpp
        include/huffman.h
        src/huffmanimage.cpp
        include/huffman_image.h)
include_directories(${CMAKE_SOURCE_DIR}/include)


configure_file(input.txt input.txt COPYONLY)
