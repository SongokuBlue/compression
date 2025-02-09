cmake_minimum_required(VERSION 3.22)
project(learn)

set(CMAKE_CXX_STANDARD 17)

add_executable(learn main.cpp
        src/huffman.cpp
        include/huffman.h
        src/huffmanImage.cpp
        include/huffmanImage.h
)
include_directories(${CMAKE_SOURCE_DIR}/include)


configure_file(input.txt input.txt COPYONLY)
configure_file(Harry_Potter.txt Harry_Potter.txt COPYONLY)
configure_file(10mb.json 10mb.json COPYONLY)