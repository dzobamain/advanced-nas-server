cmake_minimum_required(VERSION 3.10)

project(advanced-nas-server C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/main.c
    src/core.c
    src/logger.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
