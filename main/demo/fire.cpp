/*

MIT No Attribution

Copyright (c) 2024 James Bryant

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
#include "demo.h"
#include "util.h"

Fire::Fire(lv_obj_t *canvas, uint16_t width, uint16_t height) : Demo(canvas, width, height)
{
    this->canvas = canvas;
    ESP_LOGD("fire", "dimensions: (%d, %d)", width, height);
    fire = (uint8_t *)heap_caps_malloc(width * height * 2, MALLOC_CAP_DEFAULT);

    if (fire == NULL) {
        ESP_LOGE("Fire", "Failed to allocate memory for fire buffer");
        return;
    }

    memset(fire, 0, width * height * 2);

    //generate the palette
    for(int x = 0; x < 256; x++)
    {
        //hsl_to_rgb888 is used to generate colors:
        //Hue goes from 0 to 85: red to yellow
        //Saturation is always the maximum: 255
        //Lightness is 0..255 for x=0..128, and 255 for x=128..255
        lv_color_t color = hsl_to_rgb(x / 3, 255, x < 20 ? 0 : std::max(std::min(255, (x * 2) - 20), 0));
        palette[x] = color;
    }
}

void Fire::renderFrame() {
    
    //randomize the bottom row of the fire buffer
    for(int x = 0; x < width; x++) fire[((height - 1) * width) + x] = abs(32768 + rand()) % 256;
    ESP_LOGD("fire", "randomize row done.");
    
    //do the fire calculations for every pixel, from top to bottom
    for(int y = 0; y < height - 1; y++) {
        for(int x = 0; x < width; x++)
        {
            fire[(y * width) + x] =
                ((fire[(((y + 1) % height) * width) + ((x - 1 + width) % width)]
                + fire[(((y + 1) % height) * width) + (x % width)]
                + fire[(((y + 1) % height) * width) + ((x + 1) % width)]
                + fire[(((y + 2) % height) * width) + (x % width)])
                * 32) / 129;
        }
    }

    //set the drawing buffer to the fire buffer, using the palette colors
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++)
        {
            lv_canvas_set_px(canvas, x, y, palette[*(fire + (y * width) + x)]);
        }
    }
    ESP_LOGD("fire", "render frame done.\n");
}

Fire::~Fire() {
    free(fire);
}