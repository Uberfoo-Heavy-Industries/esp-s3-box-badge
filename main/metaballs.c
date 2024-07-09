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

static const uint8_t NUM_BALLS = 2;
static const uint8_t MIN_VELOCITY = 3;
static const uint8_t MAX_VELOCITY = 5;
static const uint8_t MIN_RADIUS = 8;
static const uint8_t MAX_RADIUS = 16;
static const uint8_t PIXEL_SIZE = 1;

lv_draw_rect_dsc_t metaballs_rect_dsc;

void
metaballs_init(lv_obj_t *canvas)
{
    if (PIXEL_SIZE > 1) {

    }
    
    /* Set up imaginary balls inside screen coordinates. */
    for (int16_t i = 0; i < NUM_BALLS; i++) {
        balls[i].radius = (rand() % MAX_RADIUS) + MIN_RADIUS;
        balls[i].color = 0xffff;
        balls[i].position.x = rand() % BSP_LCD_H_RES;
        balls[i].position.y = rand() % BSP_LCD_V_RES;
        balls[i].velocity.x = (rand() % MAX_VELOCITY) + MIN_VELOCITY;
        balls[i].velocity.y = (rand() % MAX_VELOCITY) + MIN_VELOCITY;
    }
}

void
metaballs_animate(lv_obj_t *canvas)
{
    for (int16_t i = 0; i < NUM_BALLS; i++) {
        balls[i].position.x += balls[i].velocity.x;
        balls[i].position.y += balls[i].velocity.y;

        /* Touch left or right edge, change direction. */
        if ((balls[i].position.x < 0) | (balls[i].position.x > BSP_LCD_H_RES)) {
            balls[i].velocity.x = balls[i].velocity.x * -1;
        }

        /* Touch top or bottom edge, change direction. */
        if ((balls[i].position.y < 0) | (balls[i].position.y > BSP_LCD_V_RES)) {
            balls[i].velocity.y = balls[i].velocity.y * -1;
        }
    }
}

/* http://www.geisswerks.com/ryan/BLOBS/blobs.html */
void
metaballs_render(lv_obj_t *canvas)
{
    const lv_color_t background = lv_color_make(0,0,0);
    const lv_color_t black = lv_color_make(0,0,0);
    const lv_color_t white = lv_color_make(255,255,255);
    const lv_color_t green = lv_color_make(0,255,0);

    lv_color_t color;

    bsp_display_lock(0);

                // lv_obj_t *rect = lv_obj_create(lv_scr_act());
                // lv_obj_set_pos(rect, 50, 50);
                // lv_obj_set_size(rect, 50, 50);
                
                // // Set the style of the rectangle
                // lv_obj_set_style_bg_color(rect, black, 0);
                // lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);  // Fully opaque
                // lv_obj_set_style_border_width(rect, 0, 0);
                // lv_obj_set_style_radius(rect, 0, 0);

    // lv_canvas_init_layer(canvas, &metaballs_layer);

    //             lv_draw_rect_dsc_init(&metaballs_rect_dsc);
    //             metaballs_rect_dsc.bg_opa = LV_OPA_COVER;
    //             metaballs_rect_dsc.bg_color = black;
    //             metaballs_rect_dsc.border_width = 0;
    //             lv_area_t coords_rect = {50, 50, 60, 60};
    //             lv_draw_rect(&metaballs_layer, &metaballs_rect_dsc, &coords_rect);

    for (uint16_t y = 0; y < BSP_LCD_V_RES; y += PIXEL_SIZE) {
        for (uint16_t x = 0; x < BSP_LCD_H_RES; x += PIXEL_SIZE) {
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
                lv_obj_t *rect = lv_obj_create(lv_scr_act());
                lv_obj_set_pos(rect, x, y);
                lv_obj_set_size(rect, PIXEL_SIZE, PIXEL_SIZE);
                
                // Set the style of the rectangle
                lv_obj_set_style_bg_color(rect, color, 0);
                lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);  // Fully opaque
                lv_obj_set_style_border_width(rect, 0, 0);
                lv_obj_set_style_radius(rect, 0, 0);
            }
        }
    }
    // lv_canvas_finish_layer(canvas, &metaballs_layer);
    bsp_display_unlock();
}
