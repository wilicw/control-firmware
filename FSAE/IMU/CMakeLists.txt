cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "fsae_imu")
project(
  ${PROJECT_NAME}
  LANGUAGES C ASM
)
add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE FSAE_IMU)

target_include_directories(${PROJECT_NAME} INTERFACE
    ./Inc
)

file(GLOB SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Src/*.c")

target_sources(${PROJECT_NAME} INTERFACE ${SRC_FILES})
