cmake_minimum_required(VERSION 3.5)
project(scanner_parser)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(scanner_parser main.cpp ${SOURCES})