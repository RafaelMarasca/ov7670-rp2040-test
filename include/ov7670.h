#ifndef _OV7670_H_
#define _OV7670_H_

#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <inttypes.h>

#define OV7670_SLAVE_ADDRESS 0x21 

typedef struct ov7670 ov7670_t;

typedef enum image_mode
{
    OV7670_VGA,
    OV7670_QVGA,
    OV7670_QCIF,
    OV7670_CIF
} ov7670_image_mode_t;

typedef enum color_mode
{
    OV7670_YUV,
    OV7670_RGB565,
    OV7670_GRAY_LEVEL
} ov7670_color_mode_t;


typedef struct ov7670_cfg
{
    uint8_t data_pins[8];
    
    uint8_t sda_pin;
    uint8_t scl_pin;
    i2c_inst_t* i2c_inst;

    uint8_t pclk_pin;
    uint8_t xclk_pin;
    uint8_t href_pin;
    uint8_t vref_pin;

    ov7670_image_mode_t image_mode;
    ov7670_color_mode_t color_mode;

} ov7670_cfg_t;

ov7670_t* ov7670_init(ov7670_cfg_t* ov7670_cfg);

int ov7670_write_reg(ov7670_t* ov7670, uint8_t reg, uint8_t data);

int ov7670_read_reg(ov7670_t* ov7670, uint8_t reg, uint8_t* data);

int ov7670_capture(ov7670_t* ov7670);

int ov7670_get_buffer_size(ov7670_t* ov7670);

uint8_t* ov7670_get_buffer(ov7670_t* ov7670);


#endif //_OV7670_H_