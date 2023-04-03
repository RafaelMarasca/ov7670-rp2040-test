#include "pico/stdlib.h"
#include "ov7670.h"
#include <stdio.h>

int main()
{
    stdio_init_all();
    ov7670_cfg_t cfg = 
    {
        .sda_pin = 0,
        .scl_pin = 1,
        .i2c_inst = i2c0,
        .xclk_pin = 2
    };

    ov7670_t* ov7670 = ov7670_init(&cfg);

    for(int i = 0; i<10; i++)
    {
        uint8_t data;
        ov7670_read_reg(ov7670, 0x11, &data);

        printf("%d \n",(int) data);
        sleep_ms(1000);
    }

    ov7670_write_reg(ov7670, 0x11, 0xFF);

    while(1)
    {
        uint8_t data;
        ov7670_read_reg(ov7670, 0x11, &data);

        printf("%d \n",(int) data);
        sleep_ms(500);
    }
   
}