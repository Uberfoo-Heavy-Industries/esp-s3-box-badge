/*

MIT No Attribution

Copyright (c) 2023 James Bryant

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

SPDX-License-Identifier: MIT-0

*/

/*
Fire Effect based on https://lodev.org/cgtutor/fire.html
*/
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_pthread.h"
#include "fire.h"

void *run(void *arg)
{
    static const int16_t h = BSP_LCD_V_RES;
    static const int16_t w = BSP_LCD_H_RES;

    fire_thrd_params *params = (fire_thrd_params *)arg;
    int rows = BSP_LCD_V_RES /2;
    int end = params->start + rows;
    uint8_t *fire = params->fire;

    //do the fire calculations for every pixel, from top to bottom
    for(int y = params->start; y < end - 1; y++) {
        for(int x = 0; x < w; x++)
        {
            fire[(y * w) + x] =
                ((fire[(((y + 1) % h) * w) + ((x - 1 + w) % w)]
                + fire[(((y + 1) % h) * w) + (x % w)]
                + fire[(((y + 1) % h) * w) + ((x + 1) % w)]
                + fire[(((y + 2) % h) * w) + (x % w)])
                * 32) / 129;
        }
    }
    return NULL;
}

Fire::Fire(lv_obj_t *canvas)
{
    this->canvas = canvas;
    fire = (uint8_t *)heap_caps_malloc(BSP_LCD_H_RES * BSP_LCD_V_RES * 2, MALLOC_CAP_DEFAULT);

    if (fire == NULL) {
        ESP_LOGE("Memory", "Failed to allocate memory");
        return;
    }

    memset(fire, 0, BSP_LCD_H_RES * BSP_LCD_V_RES * 2);
}

void Fire::init() {
    //generate the palette
    for(int x = 0; x < 256; x++)
    {
        //hsl_to_rgb888 is used to generate colors:
        //Hue goes from 0 to 85: red to yellow
        //Saturation is always the maximum: 255
        //Lightness is 0..255 for x=0..128, and 255 for x=128..255
        lv_color_t color = lv_color_hsv_to_rgb(x / 3, 255, std::min(255, x * 2));
        palette[x] = color;
    }
}

void Fire::playFire() {
    int16_t h = BSP_LCD_V_RES;
    int16_t w = BSP_LCD_H_RES;
    
    //randomize the bottom row of the fire buffer
    for(int x = 0; x < w; x++) fire[((h - 1) * w) + x] = abs(32768 + rand()) % 256;
    printf("done.\n");
    
    pthread_attr_t attr;
    pthread_t thread1, thread2;
    esp_pthread_cfg_t esp_pthread_cfg;
    int res;

    fire_thrd_params params1 = {
        .start = 0,
        .fire = fire
    };

    // Create a pthread with the default parameters
    res = pthread_create(&thread1, NULL, run, &params1);
    assert(res == 0);
    printf("Created thread 0x%" PRIx32 "\n", thread1);

    fire_thrd_params params2 = {
        .start = h / 2,
        .fire = fire
    };
    
    // Create a pthread with the default parameters
    res = pthread_create(&thread2, NULL, run, &params2);
    assert(res == 0);
    printf("Created thread 0x%" PRIx32 "\n", thread2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    //set the drawing buffer to the fire buffer, using the palette colors
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++)
        {
            lv_canvas_set_px(canvas, x, y, palette[*(fire + (y * w) + x)]);
        }
    }
    printf("done.\n");
}

