#include "pico/stdlib.h"
#include <stdio.h>
#include "img.h"

int main()
{
    stdio_init_all();

    gpio_init(24);
    gpio_set_dir(24, GPIO_IN);

    gpio_pull_up(24);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    while(!gpio_get(24));

    while(1)
    {
        while(gpio_get(24));
    
        {
            for(int i = 0; i < img_data.width*img_data.height*3; i++)
            {
                if(img_data.data[i] == '\n')
                    printf("%c", img_data.data[i]+1);
                else
                    printf("%c", img_data.data[i]);
            }
            gpio_put(25, 1);
            sleep_ms(1000);
            gpio_put(25,0);
        }    
    }   
}