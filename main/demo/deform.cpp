/*

MIT No Attribution

Copyright (c) 2021-2023 Mika Tuupola

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

Adapted from article by Inigo Quilez:
https://iquilezles.org/www/articles/deform/deform.htm

SPDX-License-Identifier: MIT-0

*/

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "bsp/esp-bsp.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "head.h"
#include "deform.h"

static const uint8_t SPEED = 2;
static const uint8_t PIXEL_SIZE = 1;

Deform::Deform(lv_obj_t * canvas, uint16_t width, uint16_t height) : Demo(canvas, width, height)
{
    int8_t *ptr = lut = (int8_t *)heap_caps_malloc(height * width * 2 * sizeof(int8_t), MALLOC_CAP_SPIRAM);
    for (uint16_t j = 0; j < height; j += PIXEL_SIZE) {
        for (uint16_t i = 0; i < width; i += PIXEL_SIZE) {

            const float x = -1.00f + 2.00f * i / width;
            const float y = -1.00f + 2.00f * j / height;
            const float r = sqrtf(x * x + y * y);
            const float a = atan2f(y, x);

            const float u = cosf(a) / r;
            const float v = sinf(a) / r;

            /* Target x and y coordinates in the texture. */
            const int8_t tx = ((int8_t)(HEAD_WIDTH * u)) % HEAD_WIDTH;
            const int8_t ty = ((int8_t)(HEAD_HEIGHT * v)) % HEAD_HEIGHT;

            *(ptr++) = tx;
            *(ptr++) = ty;
        }
    }
}

Deform::~Deform() {
    heap_caps_free(lut);
}

void Deform::renderFrame()
{
    bsp_display_lock(0);

    int8_t *ptr = lut;
    for (uint16_t y = 0; y < height; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < width; x += PIXEL_SIZE) {

            /* Retrieve texture x and y coordinates for display coordinates. */
            int16_t u = *(ptr++) + frame;
            int16_t v = *(ptr++) + frame;

            u = abs(u) % HEAD_WIDTH;
            v = abs(v) % HEAD_HEIGHT;

            /* Get the pixel from texture and put it to the screen. */
            const lv_color_t *color = (lv_color_t *) (head + HEAD_WIDTH * sizeof(lv_color_t) * v + sizeof(lv_color_t) * u);

            if (1 == PIXEL_SIZE) {
                lv_canvas_set_px_color(canvas, x, y, *color);
            } else {
                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = *color;
                lv_canvas_draw_rect(canvas, x, y, PIXEL_SIZE, PIXEL_SIZE, &rect_dsc);
            }
        }
    }
    frame = frame + SPEED;

    bsp_display_unlock();
    
}
