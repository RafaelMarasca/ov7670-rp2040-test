cmake_minimum_required(VERSION 3.24)

include(pico_sdk_import.cmake)

project(lib_ov7670)

set(TEST "false")

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    
    set(TEST "true")

    pico_sdk_init()

endif()

add_subdirectory(test)

add_library(libov7670 src/ov7670.c)

target_include_directories(libov7670 PUBLIC include)

target_link_libraries(libov7670 PUBLIC pico_stdlib hardware_pio hardware_i2c hardware_pwm hardware_dma hardware_pio)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_PROJECT_DIR}/generated)

    # generate the header file into the source tree as it is included in the RP2040 datasheet
 pico_generate_pio_header(libov7670 ${CMAKE_CURRENT_LIST_DIR}/src/capture.pio OUTPUT_DIR ${CMAKE_CURRENT_PROJECT_DIR}/generated)