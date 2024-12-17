cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME fsae-2024)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

add_compile_definitions(JSMN_STATIC)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# FSAE Libraries
add_subdirectory(SEGGER_RTT)
add_subdirectory(FSAE/ADC)
add_subdirectory(FSAE/Configs)
add_subdirectory(FSAE/GNSS)
add_subdirectory(FSAE/IMU)
add_subdirectory(FSAE/Inverter)
add_subdirectory(FSAE/Steering)
add_subdirectory(FSAE/Tasks)
add_subdirectory(FSAE/WheelSpeed)
add_subdirectory(USB_DEVICE)
add_subdirectory(STM32_USB_Device_Library)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    # Add user defined libraries
    segger_rtt
    fsae_adc
    fsae_config
    fsae_gnss
    fsae_imu
    fsae_inverter
    fsae_steering
    fsae_tasks
    fsae_wheelspeed
    usb_device
    stm32_usb_device
)
