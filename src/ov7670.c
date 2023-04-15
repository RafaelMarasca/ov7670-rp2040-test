#include "ov7670.h"
#include "ov7670_internals.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "capture.pio.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

struct ov7670
{
    uint8_t data_pins[8];
    
    uint8_t sda;
    uint8_t scl;
    i2c_inst_t* i2c_inst;

    uint8_t pclk;
    uint8_t xclk;

    uint pwm_slice_num;

    uint8_t href;
    uint8_t vref;

    ov7670_image_mode_t image_mode;
    ov7670_color_mode_t color_mode;

    int height;
    int width;

    uint8_t bytes_per_pixel;

    uint8_t* buffer;
    unsigned int buffer_size;

    unsigned int hstart;
    unsigned int hstop;
    unsigned int vstart;
    unsigned int vstop;
};


ov7670_t* ov7670_init(ov7670_cfg_t* ov7670_cfg)
{
    ov7670_t* ov7670 = NULL;

    if(ov7670_cfg != NULL)
    {
        ov7670 = (ov7670_t*) malloc(sizeof(ov7670_t));

        if(ov7670)
        {
             for(int i = 0; i < 8; i++)
            {
                ov7670->data_pins[i] = ov7670_cfg->data_pins[i];
            } 

            ov7670->i2c_inst = ov7670_cfg->i2c_inst;
            ov7670->sda = ov7670_cfg->sda_pin;
            ov7670->scl = ov7670_cfg->scl_pin;
            ov7670->pclk = ov7670_cfg->pclk_pin;
            ov7670->xclk = ov7670_cfg->xclk_pin;
            ov7670->href = ov7670_cfg->href_pin;
            ov7670->vref = ov7670_cfg->vref_pin;
        }
    }

    /* I2C configuration */
    i2c_init(ov7670->i2c_inst, 400000);
    gpio_set_function(ov7670->sda, GPIO_FUNC_I2C);
    gpio_set_function(ov7670->scl, GPIO_FUNC_I2C);
    gpio_pull_up(ov7670->sda);
    gpio_pull_up(ov7670->scl);


    /* Clock Configuration (24MHz)*/
    gpio_set_function(ov7670->xclk, GPIO_FUNC_PWM);
    ov7670->pwm_slice_num = pwm_gpio_to_slice_num(ov7670->xclk);
    pwm_set_clkdiv(ov7670->pwm_slice_num, 1.3021);
    pwm_set_wrap(ov7670->pwm_slice_num, 3);
    pwm_set_gpio_level(ov7670->xclk, 2);
    pwm_set_enabled(ov7670->pwm_slice_num, true);


    ov7670->pclk = ov7670_cfg->pclk_pin;
    ov7670->vref = ov7670_cfg->vref_pin;
    ov7670->href = ov7670_cfg->href_pin;

    gpio_init(ov7670->href);
    gpio_set_dir(ov7670->href, GPIO_IN);
    gpio_init(ov7670->vref);
    gpio_set_dir(ov7670->vref, GPIO_IN);
    gpio_init(ov7670->pclk);
    gpio_set_dir(ov7670->pclk, GPIO_IN);


    ov7670->color_mode = ov7670_cfg->color_mode;
    ov7670->image_mode = ov7670_cfg->image_mode;
    ov7670->buffer = NULL;
    ov7670->buffer_size = 0;

    ov7670_image_cfg(ov7670);

    return ov7670;
}


int ov7670_write_reg(ov7670_t* ov7670, uint8_t reg, uint8_t data)
{
    if(ov7670)
    {
        uint8_t buffer[] = {reg, data};
        return i2c_write_blocking(ov7670->i2c_inst, OV7670_SLAVE_ADDRESS, buffer, 2, false);
    }

    return -1;
}

int ov7670_read_reg(ov7670_t* ov7670, uint8_t reg, uint8_t* data)
{
    if(ov7670)
    {
        i2c_write_blocking(ov7670->i2c_inst, OV7670_SLAVE_ADDRESS, &reg, 1, false);
        return i2c_read_blocking(ov7670->i2c_inst, OV7670_SLAVE_ADDRESS, data, 1, false);
    }

    return -1;
}



int ov7670_image_cfg(ov7670_t* ov7670)
{
    if(ov7670)
    {
        switch(ov7670->image_mode)
        {
            case OV7670_VGA:
                if(ov7670_vga_cfg(ov7670))
                    return -1;
                break;
            case OV7670_QVGA:
                if(ov7670_qvga_cfg(ov7670))
                    return -1;
                break;
            case OV7670_QCIF:
                if(ov7670_qcif_cfg(ov7670))
                    return -1;
                break;
            default:
                return -1;
                break;
        }

        switch (ov7670->color_mode)
        {
            case OV7670_YUV:
                if(ov7670_yuv_cfg(ov7670))
                    return -1;
                break;
            
            case OV7670_RGB565:
                if(ov7670_rgb565_cfg(ov7670))
                    return -1;
                break;

            case OV7670_GRAY_LEVEL:
                if(ov7670_gray_cfg(ov7670))
                    return -1;
                break;

            default:
                return -1;
                break;
        }

        ov7670->buffer_size = ov7670->width * ov7670->height * ov7670->bytes_per_pixel;

        ov7670->buffer = (uint8_t*)malloc(sizeof(uint8_t) * ov7670->buffer_size);
        
        if(ov7670->buffer)
            return 0;
    }
    return -1;
}

int ov7670_yuv_cfg(ov7670_t* ov7670)
{
    if(ov7670)
    {
        ov7670->bytes_per_pixel = 2;
        return 0;
    }

    return -1;
}

int ov7670_gray_cfg(ov7670_t* ov7670)
{
    if(ov7670)
    {
        ov7670->bytes_per_pixel = 1;
        return 0;
    }

    return -1;
}

int ov7670_rgb565_cfg(ov7670_t* ov7670)
{
    return -1;
}

int ov7670_qcif_cfg(ov7670_t* ov7670)
{
    if(ov7670)
    {
        ov7670->height = 144;
        ov7670->width = 176;

        /* Empirically Determined */
        ov7670->hstart = 456;
        ov7670->hstop = 24;
        ov7670->vstart = 14;
        ov7670->vstop = 494;

        /* Reset and Initial Configuration Configuration */
        ov7670_write_reg(ov7670, 0x12, 0x80); 
        ov7670_write_reg(ov7670, 0x3A, 0x0D);
        ov7670_write_reg(ov7670, 0x12, 0x08);
        ov7670_write_reg(ov7670, 0x0C, 0x0C);

        /* Configuration for PCLK 12MHz and 24MHz input (Tweaked Empirically) (30fps)*/

        ov7670_write_reg(ov7670, 0x70, 0x3a);
        ov7670_write_reg(ov7670, 0x70, 0x35);

        ov7670_write_reg(ov7670, 0x11, 0x01);
        ov7670_write_reg(ov7670, 0x6b, 0x0a);
        ov7670_write_reg(ov7670, 0x2a, 0x00);
        ov7670_write_reg(ov7670, 0x2b, 0x00);
        ov7670_write_reg(ov7670, 0x92, 0x00);
        ov7670_write_reg(ov7670, 0x93, 0x00);
        ov7670_write_reg(ov7670, 0x3b, 0x0a);

        ov7670_write_reg(ov7670, 0x3E, 0x11);
        ov7670_write_reg(ov7670, 0x72, 0x11);
        ov7670_write_reg(ov7670, 0x73, 0xf1);
        ov7670_write_reg(ov7670, 0xa2, 0x52);

        return ov7670_commom_cfg(ov7670);
    }

    return -1;
}

int ov7670_cif_cfg(ov7670_t* ov7670)
{
    return -1;
}

int ov7670_vga_cfg(ov7670_t* ov7670)
{
    return -1;
}

int ov7670_qvga_cfg(ov7670_t* ov7670)
{
    return -1;
}

int ov7670_commom_cfg(ov7670_t* ov7670)
{
    if(ov7670)
    {
        uint8_t value  = 0;

        /* HSTART and HSTOP configuration */
        ov7670_write_reg(ov7670, 0x17, (ov7670->hstart >> 3) & 0xff);
        ov7670_write_reg(ov7670, 0x18, (ov7670->hstop >> 3) & 0xff);
        value = ov7670_read_reg(ov7670, 0x32, &value);
        value = (value & 0xC0) | ((ov7670->hstop & 0x7) << 3) | (ov7670->hstart & 0x07);
        ov7670_write_reg(ov7670, 0x32, value);

        /* VSTART and VSTOP configuration */
        ov7670_write_reg(ov7670, 0x19, (ov7670->vstart >> 2) & 0xff);
        ov7670_write_reg(ov7670, 0x1a, (ov7670->vstop>> 2) & 0xff);
        value = ov7670_read_reg(ov7670, 0x32, &value);
        value = (value & 0xF0) | ((ov7670->vstop & 0x03) << 2) | (ov7670->vstart & 0x03);
        ov7670_write_reg(ov7670, 0x03, value);

        /* Color Correction Matrix Parameters */
        ov7670_write_reg(ov7670, 0x40, 0xc0);
        ov7670_write_reg(ov7670, 0x14, 0x48); 
        ov7670_write_reg(ov7670, 0x4f, 0x80);		
        ov7670_write_reg(ov7670, 0x50, 0x80);		
        ov7670_write_reg(ov7670, 0x51, 0x00);		
        ov7670_write_reg(ov7670, 0x52, 0x22);		
        ov7670_write_reg(ov7670, 0x53, 0x5e);		
        ov7670_write_reg(ov7670, 0x54, 0x80);		
        ov7670_write_reg(ov7670, 0x3d, 0x80 | 0x40);
        ov7670_write_reg(ov7670, 0xff, 0xff);

        /* Gamma Curves Parameters */
        ov7670_write_reg(ov7670,0x7a, 0x20);		ov7670_write_reg(ov7670,0x7b, 0x10);
        ov7670_write_reg(ov7670,0x7c, 0x1e);		ov7670_write_reg(ov7670,0x7d, 0x35);
        ov7670_write_reg(ov7670,0x7e, 0x5a);		ov7670_write_reg(ov7670,0x7f, 0x69);
        ov7670_write_reg(ov7670,0x80, 0x76);		ov7670_write_reg(ov7670,0x81, 0x80);
        ov7670_write_reg(ov7670,0x82, 0x88);		ov7670_write_reg(ov7670,0x83, 0x8f);
        ov7670_write_reg(ov7670,0x84, 0x96);		ov7670_write_reg(ov7670,0x85, 0xa3);
        ov7670_write_reg(ov7670,0x86, 0xaf);		ov7670_write_reg(ov7670,0x87, 0xc4);
        ov7670_write_reg(ov7670,0x88, 0xd7);		ov7670_write_reg(ov7670,0x89, 0xe8);

        /* Automatic Gain Configuration*/
        ov7670_write_reg(ov7670, 0x13, 0x80 | 0x40 | 0x20);
        ov7670_write_reg(ov7670, 0x00, 0);	
        ov7670_write_reg(ov7670, 0x10, 0);
        ov7670_write_reg(ov7670, 0x0d, 0x40); 
        ov7670_write_reg(ov7670, 0x14, 0x18); 
        ov7670_write_reg(ov7670, 0xa5, 0x05);	
        ov7670_write_reg(ov7670, 0xab, 0x07);
        ov7670_write_reg(ov7670, 0x24, 0x95);	
        ov7670_write_reg(ov7670, 0x25, 0x33);
        ov7670_write_reg(ov7670, 0x26, 0xe3);	
        ov7670_write_reg(ov7670, 0x9f, 0x78);
        ov7670_write_reg(ov7670, 0xa0, 0x68);	
        ov7670_write_reg(ov7670, 0xa1, 0x03); 
        ov7670_write_reg(ov7670, 0xa6, 0xd8);	
        ov7670_write_reg(ov7670, 0xa7, 0xd8);
        ov7670_write_reg(ov7670, 0xa8, 0xf0);	
        ov7670_write_reg(ov7670, 0xa9, 0x90);
        ov7670_write_reg(ov7670, 0xaa, 0x94);
        ov7670_write_reg(ov7670, 0x13, 0x80 | 0x40 | 0x20 | 0x04 | 0x01);

        ov7670_write_reg(ov7670, 0x0e, 0x61);	    ov7670_write_reg(ov7670, 0x0f, 0x4b);
        ov7670_write_reg(ov7670, 0x16, 0x02);		ov7670_write_reg(ov7670, 0x1e, 0x07);
        ov7670_write_reg(ov7670, 0x21, 0x02);		ov7670_write_reg(ov7670, 0x22, 0x91);
        ov7670_write_reg(ov7670, 0x29, 0x07);		ov7670_write_reg(ov7670, 0x33, 0x0b);
        ov7670_write_reg(ov7670, 0x35, 0x0b);		ov7670_write_reg(ov7670, 0x37, 0x1d);
        ov7670_write_reg(ov7670, 0x38, 0x71);		ov7670_write_reg(ov7670, 0x39, 0x2a);
        ov7670_write_reg(ov7670, 0x3c, 0x78);	    ov7670_write_reg(ov7670, 0x4d, 0x40);
        ov7670_write_reg(ov7670, 0x4e, 0x20);		ov7670_write_reg(ov7670, 0x69, 0x00);
        ov7670_write_reg(ov7670, 0x74, 0x10);       ov7670_write_reg(ov7670, 0x8d, 0x4f);
        ov7670_write_reg(ov7670, 0x8e, 0x00);       ov7670_write_reg(ov7670, 0x8f, 0x00);
        ov7670_write_reg(ov7670, 0x90, 0x00);       ov7670_write_reg(ov7670, 0x91, 0x00);
        ov7670_write_reg(ov7670, 0x96, 0x00);       ov7670_write_reg(ov7670, 0x9a, 0x00);
        ov7670_write_reg(ov7670, 0xb0, 0x84);       ov7670_write_reg(ov7670, 0xb1, 0x0c);
        ov7670_write_reg(ov7670, 0xb2, 0x0e);       ov7670_write_reg(ov7670, 0xb3, 0x82);
        ov7670_write_reg(ov7670, 0xb8, 0x0a);

        ov7670_write_reg(ov7670, 0x43, 0x0a);		ov7670_write_reg(ov7670, 0x44, 0xf0);
        ov7670_write_reg(ov7670, 0x45, 0x34);		ov7670_write_reg(ov7670, 0x46, 0x58);
        ov7670_write_reg(ov7670, 0x47, 0x28);		ov7670_write_reg(ov7670, 0x48, 0x3a);
        ov7670_write_reg(ov7670, 0x59, 0x88);		ov7670_write_reg(ov7670, 0x5a, 0x88);
        ov7670_write_reg(ov7670, 0x5b, 0x44);		ov7670_write_reg(ov7670, 0x5c, 0x67);
        ov7670_write_reg(ov7670, 0x5d, 0x49);		ov7670_write_reg(ov7670, 0x5e, 0x0e);
        ov7670_write_reg(ov7670, 0x6c, 0x0a);		ov7670_write_reg(ov7670, 0x6d, 0x55);
        ov7670_write_reg(ov7670, 0x6e, 0x11);		ov7670_write_reg(ov7670, 0x6f, 0x9f); 
        ov7670_write_reg(ov7670, 0x6a, 0x40);		ov7670_write_reg(ov7670, 0x01, 0x40);
        ov7670_write_reg(ov7670, 0x02, 0x60);
        ov7670_write_reg(ov7670, 0x13, 0x80 | 0x40 | 0x20 | 0x04 | 0x01 | 0x02);

        ov7670_write_reg(ov7670, 0x4f, 0x80);		
        ov7670_write_reg(ov7670, 0x50, 0x80);
        ov7670_write_reg(ov7670, 0x51, 0x00);	        
        ov7670_write_reg(ov7670, 0x52, 0x22);
        ov7670_write_reg(ov7670, 0x53, 0x5e);		
        ov7670_write_reg(ov7670, 0x54, 0x80);
        ov7670_write_reg(ov7670, 0x58, 0x9e);

        ov7670_write_reg(ov7670, 0x41, 0x08);	     ov7670_write_reg(ov7670, 0x3f, 0);
        ov7670_write_reg(ov7670, 0x75, 0x05);	     ov7670_write_reg(ov7670, 0x76, 0xe1);
        ov7670_write_reg(ov7670, 0x4c, 0x00);		 ov7670_write_reg(ov7670, 0x77, 0x01);
        ov7670_write_reg(ov7670, 0x3d, 0xc3);	     ov7670_write_reg(ov7670, 0x4b, 0x09);
        ov7670_write_reg(ov7670, 0xc9, 0x60);	     ov7670_write_reg(ov7670, 0x41, 0x38);
        ov7670_write_reg(ov7670, 0x56, 0x40);

        ov7670_write_reg(ov7670, 0x34, 0x11 );		 ov7670_write_reg(ov7670, 0x3b, 0x02 | 0x10);
        ov7670_write_reg(ov7670, 0xa4, 0x88 );		 ov7670_write_reg(ov7670, 0x96, 0x00);
        ov7670_write_reg(ov7670, 0x97, 0x30 );		 ov7670_write_reg(ov7670, 0x98, 0x20);
        ov7670_write_reg(ov7670, 0x99, 0x30 );		 ov7670_write_reg(ov7670, 0x9a, 0x84);
        ov7670_write_reg(ov7670, 0x9b, 0x29 );		 ov7670_write_reg(ov7670, 0x9c, 0x03);
        ov7670_write_reg(ov7670, 0x9d, 0x4c );		 ov7670_write_reg(ov7670, 0x9e, 0x3f);
        ov7670_write_reg(ov7670, 0x78, 0x04 );

        return 0;
    }

    return -1;
}

int ov7670_capture(ov7670_t* ov7670)
{
    if(ov7670)
    {
        int index = 0;

        if(ov7670->color_mode == OV7670_GRAY_LEVEL)
        {
            while(!gpio_get(ov7670->vref));
            while(gpio_get(ov7670->vref));

            for(int i = 0; i<ov7670->height; i++)
            {
                while(!gpio_get(ov7670->href));

                for(int j = 0; j< ov7670->width * ov7670->bytes_per_pixel; j++)
                {
                    while(!gpio_get(ov7670->pclk));
                    while(gpio_get(ov7670->pclk));
                    while(!gpio_get(ov7670->pclk));
                    ov7670->buffer[index++] = (uint8_t) ((gpio_get_all()) & 0xFF);
                    while(gpio_get(ov7670->pclk));
                }
                while(gpio_get(ov7670->href));
            }

        }else
        {
            while(!gpio_get(ov7670->vref));
            while(gpio_get(ov7670->vref));

            for(int i = 0; i<ov7670->height; i++)
            {
                while(!gpio_get(ov7670->href));

                for(int j = 0; j< ov7670->width * ov7670->bytes_per_pixel; j++)
                {
                    while(!gpio_get(ov7670->pclk));
                    ov7670->buffer[index++] = (uint8_t) ((gpio_get_all()) & 0xFF);
                    while(gpio_get(ov7670->pclk));
                }
                while(gpio_get(ov7670->href));
            }
        }

        return 0;
    }

    return -1;
}

uint8_t* ov7670_get_buffer(ov7670_t* ov7670)
{
    if(ov7670)
    {
        return ov7670->buffer;
    }

    return NULL;
}

int ov7670_get_buffer_size(ov7670_t* ov7670)
{
    if(ov7670)
    {
        return ov7670->buffer_size;
    }

    return -1;
}
