#include "ov7670.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "capture.pio.h"
#include <inttypes.h>
#include <stdlib.h>


uint8_t data[176*144*2] ={};

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

    uint8_t height;
    uint8_t width;
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

    i2c_init(ov7670->i2c_inst, 400000);
    gpio_set_function(ov7670->sda, GPIO_FUNC_I2C);
    gpio_set_function(ov7670->scl, GPIO_FUNC_I2C);
    gpio_pull_up(ov7670->sda);
    gpio_pull_up(ov7670->scl);

    gpio_set_function(ov7670->xclk, GPIO_FUNC_PWM);
    ov7670->pwm_slice_num = pwm_gpio_to_slice_num(ov7670->xclk);
    pwm_set_clkdiv(ov7670->pwm_slice_num, 1.3021);
    pwm_set_wrap(ov7670->pwm_slice_num, 3);
    pwm_set_gpio_level(ov7670->xclk, 2);
    pwm_set_enabled(ov7670->pwm_slice_num, true);

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

int ov7670_get_frame();

int init_dma()
{

    uint offset = pio_add_program(pio0, &capture_program);
    capture_program_init(pio0, 0, offset, 0);

    pio_sm_put_blocking(pio0, 0, 144);
    pio_sm_put_blocking(pio0, 0, 176*2);

    int chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_PIO0_RX0);
    dma_channel_configure(
        chan,          // Channel to be configured
        &cfg,            // The configuration we just created
        data,           // The initial write address
        &pio0->rxf[0],           // The initial read address
        176*144*2, // Number of transfers; in this case each is 1 byte.
        true          // Start immediately.
    );


    dma_channel_wait_for_finish_blocking(chan);

}