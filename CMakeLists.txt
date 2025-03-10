cmake_minimum_required(VERSION 3.13)

project(codecrafters-interpreter LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

file(GLOB_RECURSE SOURCE_FILES src/*.c)
add_executable(interpreter ${SOURCE_FILES})
target_include_directories(interpreter PRIVATE src)
