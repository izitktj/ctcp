cmake_minimum_required(VERSION 3.10.0)

project(ctcp_client LANGUAGES C)

file(GLOB_RECURSE _src
  ${PROJECT_SOURCE_DIR}/src/*.c
)

add_executable(${PROJECT_NAME} ${_src})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

