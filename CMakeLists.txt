cmake_minimum_required(VERSION 3.16)

project(CTCP LANGUAGES C)

file(GLOB_RECURSE _src
    ${PROJECT_SOURCE_DIR}/src/*.c
)

add_executable(CTCP ${_src})

target_include_directories(CTCP PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
