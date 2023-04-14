#include "ov7670.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

int main()
{
    stdio_init_all();
    ov7670_cfg_t cfg = 
    {
        .sda_pin = 16,
        .scl_pin = 17,
        .i2c_inst = i2c0,
        .xclk_pin = 20,
        .color_mode = OV7670_YUV,
        .image_mode = OV7670_QCIF,
        .pclk_pin = 13,
        .href_pin = 14,
        .vref_pin = 15
    };

    ov7670_t* ov7670 = ov7670_init(&cfg);

    gpio_init(24);
    gpio_set_dir(24, GPIO_IN);
    gpio_pull_up(24);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    while(1){

        while(!gpio_get(24));
        while(gpio_get(24));

        ov7670_capture(ov7670);

        gpio_put(25, 1);

        sleep_ms(1000);

        gpio_put(25,0); 

        uint8_t* buffer = ov7670_get_buffer(ov7670);
        int size = ov7670_get_buffer_size(ov7670);

        for(int i = 0; i < size ; i++)
        {
            if(buffer[i] == '\n')
                printf("%c", buffer[i]+1);
            else
                printf("%c", buffer[i]);
        }

             
    }
}