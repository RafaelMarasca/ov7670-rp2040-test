cmake_minimum_required(VERSION 3.24)

add_executable(img_tx)

target_sources(img_tx PRIVATE img_tx.c)

target_link_libraries(img_tx PRIVATE libov7670 pico_stdlib)

pico_add_extra_outputs(img_tx)