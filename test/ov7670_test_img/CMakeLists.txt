cmake_minimum_required(VERSION 3.24)

add_executable(capture)

target_sources(capture PRIVATE test.c)

target_link_libraries(capture PRIVATE libov7670 hardware_gpio pico_stdlib)

pico_enable_stdio_usb(capture 1)
pico_enable_stdio_uart(capture 0)

pico_add_extra_outputs(capture)