#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "bsp/esp-bsp.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "lvgl.h"

#include "main.h"
#include "metaballs.h"
#include "fire.h"

#define DISP_BUF_SIZE BSP_LCD_H_RES * BSP_LCD_V_RES

static char *TAG = "app_main";

void metaballs_task(void *param) {
    lv_color_t pallette[256];
    uint8_t *fire = (uint8_t *)heap_caps_malloc(BSP_LCD_H_RES * BSP_LCD_V_RES * 2, MALLOC_CAP_DEFAULT);
    memset(fire, 0, BSP_LCD_H_RES * BSP_LCD_V_RES * 2);

    if (fire == NULL) {
        ESP_LOGE("Memory", "Failed to allocate memory");
        return;
    }

    // while (true) {
    //     metaballs_animate((lv_obj_t*)param);
    //     metaballs_render((lv_obj_t*)param);
    // }

    Fire *fire_demo = new Fire((lv_obj_t *)param);
    fire_demo->init();
    printf("fire init\n");
    while (true) {
        bsp_display_lock(0);
        printf("fire frame\n");
        fire_demo->playFire();
        bsp_display_unlock();
    }
}

extern "C" int app_main()
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

     /* Mount SPIFFS */
    bsp_spiffs_mount();

    bsp_display_lock(0);

    // LV_DRAW_BUF_DEFINE(draw_buf_16bpp, BSP_LCD_H_RES, BSP_LCD_V_RES, LV_COLOR_FORMAT_RGB565);

    // lv_obj_t *canvas = lv_canvas_create(lv_screen_active());
    // lv_canvas_set_draw_buf(canvas, &draw_buf_16bpp);
    // lv_obj_center(canvas);
    // lv_canvas_fill_bg(canvas, lv_color_make(0, 255, 0), LV_OPA_COVER);

    
        ESP_LOGI(TAG, "\tDescription\tInternal\tSPIRAM");
        ESP_LOGI(TAG, "Current Free Memory\t%d\t\t%d",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                 heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        ESP_LOGI(TAG, "Min. Ever Free Size\t%d\t\t%d",
                 heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
                 heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));

    //metaballs_init(canvas);

    //xTaskCreate(metaballs_task, "metaballs", 4096, canvas, tskIDLE_PRIORITY, NULL);

    FILE *file = fopen("/spiffs/emoji.png", "r");
    if (file) {
        fclose(file);
    } else {
        ESP_LOGE("SPIFFS", "File not found: /spiffs/emoji.png");
    }

    lv_obj_t *img = lv_img_create(lv_scr_act());
    
    /* Set src of image with file name */
    lv_img_set_src(img, "S:/spiffs/lazer.png");

    /* Align object */
    lv_obj_center(img);

    bsp_display_unlock();

    return 0;
}
