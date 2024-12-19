cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "fsae_wheelspeed")
project(
  ${PROJECT_NAME}
  LANGUAGES C ASM
)
add_library(${PROJECT_NAME} INTERFACE)

add_compile_definitions()

target_compile_definitions(${PROJECT_NAME} INTERFACE FSAE_WHEELSPEED)

target_include_directories(${PROJECT_NAME} INTERFACE
    ./Inc
)

file(GLOB SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Src/*.c")

target_sources(${PROJECT_NAME} INTERFACE ${SRC_FILES})
