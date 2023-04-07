if(${TEST} STREQUAL "true")

    add_subdirectory(ov7670_i2c)
    add_subdirectory(ov7670_img_tx)
    add_subdirectory(ov7670_test_pattern)
    add_subdirectory(ov7670_img_capture)

endif()