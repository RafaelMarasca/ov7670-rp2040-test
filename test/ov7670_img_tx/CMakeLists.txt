cmake_minimum_required(VERSION 3.24)

add_executable(img_tx)

target_sources(img_tx PRIVATE img_tx.c)

target_link_libraries(img_tx PRIVATE pico_stdlib)

pico_enable_stdio_usb(img_tx 1)
pico_enable_stdio_uart(img_tx 0)

pico_add_extra_outputs(img_tx)