cmake_minimum_required(VERSION 3.26)
project(coro C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(coro src/main.c
        src/fiber.c)
