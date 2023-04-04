cmake_minimum_required(VERSION 3.24)

add_executable(ov7670_i2c)

target_sources(ov7670_i2c PRIVATE ov7670_i2c.c)

target_link_libraries(ov7670_i2c PRIVATE libov7670 pico_stdlib)

pico_add_extra_outputs(ov7670_i2c)