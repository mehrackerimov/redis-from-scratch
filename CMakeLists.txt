cmake_minimum_required(VERSION 3.20)

project(redis_from_scratch)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(redis_from_scratch
    src/main.cpp
    src/database.cpp
    src/user_manager.cpp
    src/command_handler.cpp
    src/server.cpp
    src/tokenizer.cpp
    src/command_parser.cpp
    src/logger.cpp
    src/config.cpp
    src/password_hasher.cpp
)

target_include_directories(redis_from_scratch PRIVATE
    include
    C:/vcpkg/installed/x64-windows/include
)

target_link_directories(redis_from_scratch PRIVATE
    C:/vcpkg/installed/x64-windows/lib
)

target_link_libraries(redis_from_scratch PRIVATE
    ws2_32
    libsodium
)