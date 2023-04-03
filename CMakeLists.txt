cmake_minimum_required(VERSION 3.24)

add_subdirectory(test)

add_library(libov7670 src/ov7670.c)

target_include_directories(libov7670 PUBLIC include)

target_link_libraries(libov7670 PUBLIC pico_stdlib hardware_pio hardware_i2c hardware_pwm)
