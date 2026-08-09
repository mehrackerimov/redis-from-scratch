cmake_minimum_required(VERSION 3.20)

project(redis_from_scratch)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(redis_from_scratch
    src/main.cpp
)

target_include_directories(redis_from_scratch PRIVATE include)