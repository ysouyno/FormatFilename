add_definitions(-DUNICODE -D_UNICODE)
cmake_minimum_required(VERSION 2.6)
project(FormatFilename)
add_executable(FormatFilename src/Main.cpp src/FormatFilename.cpp)