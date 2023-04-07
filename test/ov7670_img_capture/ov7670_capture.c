#include "ov7670.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "ov7670_capture.h"

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
   	uint hstart		= 456; /* Empirically determined */
	uint hstop		=  24;
	uint vstart		=  14;
	uint vstop		= 494;

    ov7670_write_reg(ov7670, 0x12, 0x80); //Reset
    ov7670_write_reg(ov7670, 0x3A, 0x0C); 
    ov7670_write_reg(ov7670, 0x12, 0x08); //Reset
    ov7670_write_reg(ov7670, 0x11, 7); //Reset
    ov7670_write_reg(ov7670, 0x0C, 0x1C); //Reset
    ov7670_read_reg(ov7670, 0x71, &value);
    //ov7670_write_reg(ov7670, 0x71, value | 0x80); //Reset
    ov7670_read_reg(ov7670, 0x76, &value);
    ov7670_write_reg(ov7670, 0x76, value | 0x30); //Reset
    ov7670_write_reg(ov7670, 0x15, 0x20); //Reset
    ov7670_read_reg(ov7670, 0x70, &value);
    ov7670_write_reg(ov7670, 0x70, value & 0x7F); //Reset

    ov7670_write_reg(ov7670,0x3E, 0x19);
    ov7670_write_reg(ov7670,0x72, 0x00);		
    ov7670_write_reg(ov7670,0x73, 0xf1);
	ov7670_write_reg(ov7670,0xa2, 0x02);

    ov7670_write_reg(ov7670,REG_SCALING_XSC, 0x40);
    ov7670_write_reg(ov7670, REG_SCALING_YSC, 0x40);




    //brilho cfg

    
    ov7670_write_reg(ov7670, REG_HSTART, (hstart >> 3) & 0xff);
    ov7670_write_reg(ov7670, REG_HSTOP, (hstop >> 3) & 0xff);
    value = ov7670_read_reg(ov7670, REG_HREF, &value);
    value = (value & 0xC0) | ((hstop & 0x7) << 3) | (hstart & 0x7);
    ov7670_write_reg(ov7670, REG_HREF, value);

    ov7670_write_reg(ov7670, REG_VSTART, (vstart >> 2) & 0xff);
    ov7670_write_reg(ov7670, REG_VSTOP, (vstop>> 2) & 0xff);
    value = ov7670_read_reg(ov7670, REG_HREF, &value);
    value = (value & 0xF0) | ((vstop & 0x3) << 2) | (vstart & 0x3);
    ov7670_write_reg(ov7670, REG_VREF, value);

    ov7670_write_reg(ov7670, REG_COM15, COM15_R00FF);
	ov7670_write_reg(ov7670, REG_COM9, 0x48); /* 32x gain ceiling; 0x8 is reserved bit */
	ov7670_write_reg(ov7670, 0x4f, 0x80 );		/* "matrix coefficient 1" */
	ov7670_write_reg(ov7670, 0x50, 0x80 );		/* "matrix coefficient 2" */
	ov7670_write_reg(ov7670, 0x51, 0    );		/* vb */
	ov7670_write_reg(ov7670, 0x52, 0x22 );		/* "matrix coefficient 4" */
	ov7670_write_reg(ov7670, 0x53, 0x5e );		/* "matrix coefficient 5" */
	ov7670_write_reg(ov7670, 0x54, 0x80 );		/* "matrix coefficient 6" */
	ov7670_write_reg(ov7670, REG_COM13, COM13_GAMMA|COM13_UVSAT);
	ov7670_write_reg(ov7670, 0xff, 0xff);

    ov7670_write_reg(ov7670,0x7a, 0x20);		ov7670_write_reg(ov7670,0x7b, 0x10);
	ov7670_write_reg(ov7670,0x7c, 0x1e);		ov7670_write_reg(ov7670,0x7d, 0x35);
	ov7670_write_reg(ov7670,0x7e, 0x5a);		ov7670_write_reg(ov7670,0x7f, 0x69);
	ov7670_write_reg(ov7670,0x80, 0x76);		ov7670_write_reg(ov7670,0x81, 0x80);
	ov7670_write_reg(ov7670,0x82, 0x88);		ov7670_write_reg(ov7670,0x83, 0x8f);
	ov7670_write_reg(ov7670,0x84, 0x96);		ov7670_write_reg(ov7670,0x85, 0xa3);
	ov7670_write_reg(ov7670,0x86, 0xaf);		ov7670_write_reg(ov7670,0x87, 0xc4);
	ov7670_write_reg(ov7670,0x88, 0xd7);		ov7670_write_reg(ov7670,0x89, 0xe8);

	/* AGC and AEC parameters.  Note we start by disabling those features,
	   then turn them only after tweaking the values. */
	ov7670_write_reg(ov7670,REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT);
	ov7670_write_reg(ov7670,REG_GAIN, 0);	ov7670_write_reg(ov7670,REG_AECH, 0 );
	ov7670_write_reg(ov7670,REG_COM4, 0x40 ); /* magic reserved bit */
	ov7670_write_reg(ov7670,REG_COM9, 0x18 ); /* 4x gain + magic rsvd bit */
	ov7670_write_reg(ov7670,REG_BD50MAX, 0x05);	ov7670_write_reg(ov7670, REG_BD60MAX, 0x07);
	ov7670_write_reg(ov7670,REG_AEW, 0x95);	ov7670_write_reg(ov7670, REG_AEB, 0x33);
	ov7670_write_reg(ov7670,REG_VPT, 0xe3);	ov7670_write_reg(ov7670, REG_HAECC1, 0x78);
	ov7670_write_reg(ov7670,REG_HAECC2, 0x68);	ov7670_write_reg(ov7670, 0xa1, 0x03); /* magic */
	ov7670_write_reg(ov7670,REG_HAECC3, 0xd8);	ov7670_write_reg(ov7670, REG_HAECC4, 0xd8);
	ov7670_write_reg(ov7670,REG_HAECC5, 0xf0);	ov7670_write_reg(ov7670, REG_HAECC6, 0x90);
	ov7670_write_reg(ov7670,REG_HAECC7, 0x94);
	ov7670_write_reg(ov7670,REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC);

	/* Almost all of these are magic "reserved" values.  */
	ov7670_write_reg(ov7670, REG_COM5, 0x61);	ov7670_write_reg(ov7670,REG_COM6, 0x4b);
	ov7670_write_reg(ov7670, 0x16, 0x02);		ov7670_write_reg(ov7670,REG_MVFP, 0x07);
	ov7670_write_reg(ov7670, 0x21, 0x02);		ov7670_write_reg(ov7670,0x22, 0x91 );
	ov7670_write_reg(ov7670, 0x29, 0x07);		ov7670_write_reg(ov7670,0x33, 0x0b );
	ov7670_write_reg(ov7670, 0x35, 0x0b);		ov7670_write_reg(ov7670,0x37, 0x1d );
	ov7670_write_reg(ov7670, 0x38, 0x71);		ov7670_write_reg(ov7670,0x39, 0x2a );
	ov7670_write_reg(ov7670, REG_COM12, 0x78);	ov7670_write_reg(ov7670,0x4d, 0x40 );
	ov7670_write_reg(ov7670, 0x4e, 0x20 );		ov7670_write_reg(ov7670,REG_GFIX, 0);
	ov7670_write_reg(ov7670, 0x6b, 0x4a );		ov7670_write_reg(ov7670,0x74, 0x10 );
   // ov7670_write_reg(ov7670, 0x6b, 0x0 );		ov7670_write_reg(ov7670,0x74, 0x10 );
	ov7670_write_reg(ov7670, 0x8d, 0x4f );		ov7670_write_reg(ov7670,0x8e, 0    );
	ov7670_write_reg(ov7670, 0x8f, 0    );		ov7670_write_reg(ov7670,0x90, 0    );
	ov7670_write_reg(ov7670, 0x91, 0    );		ov7670_write_reg(ov7670,0x96, 0    );
	ov7670_write_reg(ov7670, 0x9a, 0    );		ov7670_write_reg(ov7670,0xb0, 0x84 );
	ov7670_write_reg(ov7670, 0xb1, 0x0c );		ov7670_write_reg(ov7670,0xb2, 0x0e );
	ov7670_write_reg(ov7670, 0xb3, 0x82 );		ov7670_write_reg(ov7670,0xb8, 0x0a );

	/* More reserved magic, some of which tweaks white balance */
	ov7670_write_reg(ov7670,0x43, 0x0a );		ov7670_write_reg(ov7670,0x44, 0xf0);
	ov7670_write_reg(ov7670,0x45, 0x34 );		ov7670_write_reg(ov7670,0x46, 0x58);
	ov7670_write_reg(ov7670,0x47, 0x28 );		ov7670_write_reg(ov7670,0x48, 0x3a);
	ov7670_write_reg(ov7670,0x59, 0x88 );		ov7670_write_reg(ov7670,0x5a, 0x88);
	ov7670_write_reg(ov7670,0x5b, 0x44 );		ov7670_write_reg(ov7670,0x5c, 0x67);
	ov7670_write_reg(ov7670,0x5d, 0x49 );		ov7670_write_reg(ov7670,0x5e, 0x0e);
	ov7670_write_reg(ov7670,0x6c, 0x0a );		ov7670_write_reg(ov7670,0x6d, 0x55);
	ov7670_write_reg(ov7670,0x6e, 0x11 );		ov7670_write_reg(ov7670,0x6f, 0x9f); /* "9e for advance AWB" */
	ov7670_write_reg(ov7670,0x6a, 0x40 );		ov7670_write_reg(ov7670,REG_BLUE, 0x40);
	ov7670_write_reg(ov7670,REG_RED, 0x60);
	ov7670_write_reg(ov7670,REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB );

	/* Matrix coefficients */
	ov7670_write_reg(ov7670, 0x4f, 0x80);		ov7670_write_reg(ov7670,0x50, 0x80);
	ov7670_write_reg(ov7670, 0x51, 0);	        ov7670_write_reg(ov7670,0x52, 0x22);
	ov7670_write_reg(ov7670, 0x53, 0x5e);		ov7670_write_reg(ov7670,0x54, 0x80);
	ov7670_write_reg(ov7670,0x58, 0x9e);

	ov7670_write_reg(ov7670,REG_COM16, COM16_AWBGAIN);	ov7670_write_reg(ov7670,REG_EDGE, 0);
	ov7670_write_reg(ov7670,0x75, 0x05);	            ov7670_write_reg(ov7670,0x76, 0xe1);
	ov7670_write_reg(ov7670,0x4c, 0);		            ov7670_write_reg(ov7670,0x77, 0x01);
	ov7670_write_reg(ov7670,REG_COM13, 0xc3);	        ov7670_write_reg(ov7670,0x4b, 0x09);
	ov7670_write_reg(ov7670,0xc9, 0x60);	            ov7670_write_reg(ov7670,REG_COM16, 0x38);
	ov7670_write_reg(ov7670,0x56, 0x40);

	ov7670_write_reg(ov7670, 0x34, 0x11 );		 ov7670_write_reg(ov7670,REG_COM11, COM11_EXP|COM11_HZAUTO );
	ov7670_write_reg(ov7670, 0xa4, 0x88 );		 ov7670_write_reg(ov7670,0x96, 0);
	ov7670_write_reg(ov7670, 0x97, 0x30 );		 ov7670_write_reg(ov7670,0x98, 0x20);
	ov7670_write_reg(ov7670, 0x99, 0x30 );		 ov7670_write_reg(ov7670,0x9a, 0x84);
	ov7670_write_reg(ov7670, 0x9b, 0x29 );		 ov7670_write_reg(ov7670,0x9c, 0x03);
	ov7670_write_reg(ov7670, 0x9d, 0x4c );		 ov7670_write_reg(ov7670,0x9e, 0x3f);
	ov7670_write_reg(ov7670, 0x78, 0x04 );

    /* Extra-weird stuff.  Some sort of multiplexor register */
	ov7670_write_reg(ov7670,0x79, 0x01);		ov7670_write_reg( ov7670,0xc8, 0xf0);
	ov7670_write_reg(ov7670,0x79, 0x0f);		ov7670_write_reg( ov7670,0xc8, 0x00);
	ov7670_write_reg(ov7670,0x79, 0x10);		ov7670_write_reg( ov7670,0xc8, 0x7e);
	ov7670_write_reg(ov7670,0x79, 0x0a);		ov7670_write_reg( ov7670,0xc8, 0x80);
	ov7670_write_reg(ov7670,0x79, 0x0b);		ov7670_write_reg( ov7670,0xc8, 0x01);
	ov7670_write_reg(ov7670,0x79, 0x0c);		ov7670_write_reg( ov7670,0xc8, 0x0f);
	ov7670_write_reg(ov7670,0x79, 0x0d);		ov7670_write_reg( ov7670,0xc8, 0x20);
	ov7670_write_reg(ov7670,0x79, 0x09);		ov7670_write_reg( ov7670,0xc8, 0x80);
	ov7670_write_reg(ov7670,0x79, 0x02);		ov7670_write_reg( ov7670,0xc8, 0xc0);
	ov7670_write_reg(ov7670,0x79, 0x03);		ov7670_write_reg( ov7670,0xc8, 0x40);
	ov7670_write_reg(ov7670,0x79, 0x05);		ov7670_write_reg( ov7670,0xc8, 0x30);
	ov7670_write_reg(ov7670,0x79, 0x26);





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