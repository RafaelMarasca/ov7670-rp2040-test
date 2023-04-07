cmake_minimum_required(VERSION 3.24)

add_executable(test_pattern)

target_sources(test_pattern PRIVATE ov7670_test_pattern.c)

target_link_libraries(test_pattern PRIVATE libov7670 hardware_gpio pico_stdlib)

pico_enable_stdio_usb(test_pattern 1)
pico_enable_stdio_uart(test_pattern 0)

pico_add_extra_outputs(test_pattern)