cmake_minimum_required(VERSION 3.10)

project(advanced-nas-server C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(SQLite3 REQUIRED)

include_directories(include)

set(SOURCES
    src/main.c
    src/core.c
    src/logger.c
    src/db.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(advanced-nas-server PRIVATE
    ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(advanced-nas-server
    SQLite::SQLite3
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
