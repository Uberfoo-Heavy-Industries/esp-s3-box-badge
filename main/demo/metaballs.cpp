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
#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "esp_random.h"

#include "metaballs.h"

struct vector2 {
    int16_t x;
    int16_t y;
};

struct ball {
    struct vector2 position;
    struct vector2 velocity;
    uint16_t radius;
    uint16_t color;
};

struct ball balls[16];

static const uint8_t NUM_BALLS = 3;
static const uint8_t MIN_VELOCITY = 3;
static const uint8_t MAX_VELOCITY = 5;
static const uint8_t MIN_RADIUS = 8;
static const uint8_t MAX_RADIUS = 16;
static const uint8_t PIXEL_SIZE = 2;

lv_draw_rect_dsc_t metaballs_rect_dsc;

Metaballs::Metaballs(lv_obj_t *canvas, uint16_t width, uint16_t height) : Demo(canvas, width, height)
{
    srand(esp_random());
    /* Set up imaginary balls inside screen coordinates. */
    for (int16_t i = 0; i < NUM_BALLS; i++) {
        balls[i].radius = (rand() % MAX_RADIUS) + MIN_RADIUS;
        balls[i].color = 0xffff;
        balls[i].position.x = rand() % width;
        balls[i].position.y = rand() % height;
        balls[i].velocity.x = (rand() % MAX_VELOCITY) + MIN_VELOCITY;
        balls[i].velocity.y = (rand() % MAX_VELOCITY) + MIN_VELOCITY;
    }
}

void
Metaballs::renderFrame() 
{
    for (int16_t i = 0; i < NUM_BALLS; i++) {
        balls[i].position.x += balls[i].velocity.x;
        balls[i].position.y += balls[i].velocity.y;

        /* Touch left or right edge, change direction. */
        if ((balls[i].position.x < 0) | (balls[i].position.x > width)) {
            balls[i].velocity.x = balls[i].velocity.x * -1;
        }

        /* Touch top or bottom edge, change direction. */
        if ((balls[i].position.y < 0) | (balls[i].position.y > height)) {
            balls[i].velocity.y = balls[i].velocity.y * -1;
        }
    }

    lv_color_t color;

    bsp_display_lock(0);

    for (uint16_t y = 0; y < height; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < width; x += PIXEL_SIZE) {
            float sum = 0;
            for (uint8_t i = 0; i < NUM_BALLS; i++) {
                const float dx = x - balls[i].position.x;
                const float dy = y - balls[i].position.y;
                const float d2 = dx * dx + dy * dy;
                sum += balls[i].radius * balls[i].radius / d2;
                // sum += balls[i].radius / sqrt(d2);
            }

            if (sum > 0.65) {
                color = black;
            } else if (sum > 0.5) {
                color = white;
            } else if (sum > 0.4) {
                color = green;
            } else {
                color = background;
            }

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
