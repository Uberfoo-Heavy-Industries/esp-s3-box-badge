#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "bsp/esp-bsp.h"
#include "lvgl.h"

#include "main.h"
#include "metaballs.h"

#define DISP_BUF_SIZE BSP_LCD_H_RES * BSP_LCD_V_RES


void metaballs_task(void *param) {
    while (true) {
        metaballs_animate((lv_obj_t*)param);
        metaballs_render((lv_obj_t*)param);
    }
}

void app_main(void)
{
    printf("startup...\n");

    /* Initialize I2C (for touch and audio) */
    bsp_i2c_init();
    /* Initialize display and LVGL */
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
        }
    };
    bsp_display_start_with_config(&cfg);

    /* Set display brightness to 100% */
    bsp_display_backlight_on();

    bsp_display_lock(0);

    LV_DRAW_BUF_DEFINE(draw_buf_16bpp, BSP_LCD_H_RES, BSP_LCD_V_RES, LV_COLOR_FORMAT_RGB565);

    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf_16bpp);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_color_make(0, 255, 0), LV_OPA_COVER);

    metaballs_init(canvas);

    xTaskCreate(metaballs_task, "metaballs", 4096, canvas, tskIDLE_PRIORITY, NULL);

    bsp_display_unlock();

}