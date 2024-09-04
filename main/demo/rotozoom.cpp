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

#include <stdlib.h>
#include <math.h>

#include "bsp/esp-bsp.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_random.h"

#include "rotozoom.h"

#include "head.h"

static const uint8_t SPEED = 2;
static const uint8_t PIXEL_SIZE = 2;

Rotozoom::Rotozoom(lv_obj_t * canvas, uint16_t width, uint16_t height) : Demo(canvas, width, height) {
   
    /* Generate look up tables. */
    for (uint16_t i = 0; i < 360; i++) {
        sinlut[i] = sin(i * M_PI / 180);
        coslut[i] = cos(i * M_PI / 180);
    }

    angle = esp_random() * ((double)360 / (double)UINT32_MAX);
}

Rotozoom::~Rotozoom() {

}

void Rotozoom::renderFrame() {
    float s, c, z;

    s = sinlut[angle];
    c = coslut[angle];
    z = s * 1.2;

    bsp_display_lock(0);

    for (uint16_t x = 0; x < width; x = x + PIXEL_SIZE) {
        for (uint16_t y = 0; y < height; y = y + PIXEL_SIZE) {

            /* Get a rotated pixel from the head image. */
            int16_t u = (int16_t)((x * c - y * s) * z) % HEAD_WIDTH;
            int16_t v = (int16_t)((x * s + y * c) * z) % HEAD_HEIGHT;

            u = abs(u);
            if (v < 0) {
                v += HEAD_HEIGHT;
            }
            lv_color_t *color = (lv_color_t *) (head + HEAD_WIDTH * sizeof(lv_color_t) * v + sizeof(lv_color_t) * u);

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

    bsp_display_unlock();

    angle = (angle + SPEED) % 360;
}