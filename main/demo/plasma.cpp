/*

MIT No Attribution

Copyright (c) 2020-2023 Mika Tuupola

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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "bsp/esp-bsp.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "plasma.h"

static const uint8_t SPEED = 4;
static const uint8_t PIXEL_SIZE = 4;

Plasma::Plasma(lv_obj_t * canvas, uint16_t width, uint16_t height) : Demo(canvas, width, height)
{
    uint8_t *ptr = plasma = (uint8_t *)heap_caps_malloc(width * height * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    palette = (lv_color_t *)heap_caps_malloc(256 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    
    /* Generate nice continous palette. */
    for(uint16_t i = 0; i < 256; i++) {
        const uint8_t r = 128.0f + 128.0f * sin((M_PI * i / 128.0f) + 1);
        const uint8_t g = 128.0f + 128.0f * sin((M_PI * i / 64.0f) + 1);
        const uint8_t b = 64;
        palette[i] = lv_color_make(r, g, b);
    }

    for (uint16_t y = 0; y < height; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < width; x += PIXEL_SIZE) {
            /* Generate three different sinusoids. */
            const float v1 = 128.0f + (128.0f * sin(x / 32.0f));
            const float v2 = 128.0f + (128.0f * sin(y / 24.0f));
            const float v3 = 128.0f + (128.0f * sin(sqrt(x * x + y * y) / 24.0f));
            /* Calculate average of the three sinusoids */
            /* and use it as color index. */
            const uint8_t color = (v1 + v2 + v3) / 3;
            *(ptr++) = color;
        }
    }
}

Plasma::~Plasma() {
    heap_caps_free(plasma);
    heap_caps_free(palette);
}

void Plasma::renderFrame()
{
    uint8_t *ptr = plasma;
    for (uint16_t y = 0; y < height; y = y + PIXEL_SIZE) {
        for (uint16_t x = 0; x < width; x = x + PIXEL_SIZE) {
            /* Get a color from plasma and choose the next color. */
            /* Unsigned integers wrap automatically. */
            const uint8_t index = *ptr + SPEED;
            /* Put the new color back to the plasma buffer. */
            *(ptr++) = index;
        }
    }

    bsp_display_lock(0);

    for (uint16_t y = 0; y < height; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < width; x += PIXEL_SIZE) {
            /* Get a color for pixel from the plasma buffer. */
            const uint8_t index = *(ptr++);
            const lv_color_t color = palette[index];
            /* Put a pixel to the display. */
            if (1 == PIXEL_SIZE) {
                lv_canvas_set_px(canvas, x, y, color);
            } else {
                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = color;
                lv_canvas_draw_rect(canvas, x, y, PIXEL_SIZE, PIXEL_SIZE, &rect_dsc);
            }
        }
    }

    bsp_display_unlock();
    
}
