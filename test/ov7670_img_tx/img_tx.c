#include "pico/stdlib.h"
#include <stdio.h>
#include "IMG.h"

int main()
{
    stdio_init_all();

    gpio_init(2);
    gpio_set_dir(2, GPIO_IN);

    gpio_pull_up(2);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    while(1)
    {
        if(gpio_get(2) == 0)
        {
            for(int i = 0; i<57600; i++)
            {
                if(shrek[i] == '\n')
                    printf("%c", shrek[i]+1);
                else
                    printf("%c", shrek[i]);
            }
            //fwrite(test, 31, 31, stdout);
            gpio_put(25, 1);
        }    
    }
   
}