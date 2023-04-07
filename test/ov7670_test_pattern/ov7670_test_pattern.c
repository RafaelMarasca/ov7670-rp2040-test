#include "ov7670.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

#define HREF 14 
#define VSYNC 15
#define PCLK 13

int main()
{

    stdio_init_all();
    ov7670_cfg_t cfg = 
    {
        .sda_pin = 16,
        .scl_pin = 17,
        .i2c_inst = i2c0,
        .xclk_pin = 20
    };

    ov7670_t* ov7670 = ov7670_init(&cfg);

     uint8_t value = 0;

    ov7670_write_reg(ov7670, 0x12, 0x80); //Reset
    ov7670_write_reg(ov7670, 0x3A, 0x0D); 
    ov7670_write_reg(ov7670, 0x12, 0x08); //Reset
    ov7670_write_reg(ov7670, 0x11, 0x01); //Reset
    ov7670_write_reg(ov7670, 0x0C, 0x00); //Reset
    ov7670_read_reg(ov7670, 0x71, &value);
    ov7670_write_reg(ov7670, 0x71, value | 0x80); //Reset
    ov7670_read_reg(ov7670, 0x76, &value);
    ov7670_write_reg(ov7670, 0x76, value | 0x30); //Reset
    ov7670_write_reg(ov7670, 0x15, 0x20); //Reset
    ov7670_read_reg(ov7670, 0x70, &value);
    ov7670_write_reg(ov7670, 0x70, value & 0x7F); //Reset


    //init_dma();

    gpio_init(24);
    gpio_set_dir(24, GPIO_IN);
    gpio_pull_up(24);

    gpio_init(HREF);
    gpio_set_dir(HREF, GPIO_IN);
    gpio_init(VSYNC);
    gpio_set_dir(VSYNC, GPIO_IN);
    gpio_init(PCLK);
    gpio_set_dir(PCLK, GPIO_IN);


    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);


    int HEIGHT = 144;
    int WIDTH = 176; 

    while(!gpio_get(24));

    while(1){

        int ind = 0; 

        while(gpio_get(24));

        while(!gpio_get(VSYNC));
        while(gpio_get(VSYNC));

        for(int i = 0; i<HEIGHT; i++)
        {
            while(!gpio_get(HREF));

            for(int j = 0; j<WIDTH*2; j++)
            {
                while(!gpio_get(PCLK));
                data[ind++] = (uint8_t) ((gpio_get_all()) & 0xFF);
                while(gpio_get(PCLK));
            }
            while(gpio_get(HREF));
        }

        
        for(int i = 0; i<(176*144*2); i++)
        {
            if(data[i] == '\n')
                printf("%c", data[i]+1);
            else
                printf("%c", data[i]);
        }

        gpio_put(25, 1);

        sleep_ms(1000);

        gpio_put(25,0);
    }
}