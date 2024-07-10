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

typedef struct demo_task_params {
    lv_obj_t *canvas;
    uint16_t width;
    uint16_t height;
} demo_task_params;

void demo_task(void *obj) {

    demo_task_params *params = (demo_task_params *)obj;
    ESP_LOGD("demo_task", "dimensions: (%d, %d)", params->width, params->height);
    Demo *demo = new Fire(params->canvas, params->width, params->height);
    while (true) {
        bsp_display_lock(0);
        demo->renderFrame();
        bsp_display_unlock();
    }
}

extern "C" int app_main()
{
    ESP_LOGD("main", "startup...");

      /* Initialize I2C (for touch and audio) */
    bsp_i2c_init();

    /* Initialize display and LVGL */
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
            .buff_spiram = 0
        }
    };
    bsp_display_start_with_config(&cfg);

    /* Set display brightness to 100% */
    bsp_display_backlight_on();

     /* Mount SPIFFS */
    bsp_spiffs_mount();

    bsp_display_lock(0);

    lv_color_t *buf = (lv_color_t *)heap_caps_malloc(LV_CANVAS_BUF_SIZE_TRUE_COLOR(BSP_LCD_H_RES, BSP_LCD_V_RES / 2), MALLOC_CAP_DEFAULT);

    lv_obj_t *canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, buf, BSP_LCD_H_RES, BSP_LCD_V_RES / 2, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align_to(canvas, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_canvas_fill_bg(canvas, lv_color_make(0, 255, 0), LV_OPA_COVER);

    ESP_LOGI("main", "\tDescription\tInternal\tSPIRAM");
    ESP_LOGI("main", "Current Free Memory\t%d\t\t%d",
                heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI("main", "Min. Ever Free Size\t%d\t\t%d",
                heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));

    demo_task_params *params = (demo_task_params *)heap_caps_malloc(sizeof(demo_task_params), MALLOC_CAP_INTERNAL);
    *params = {
        .canvas = canvas,
        .width = BSP_LCD_H_RES,
        .height = BSP_LCD_V_RES / 2
    };

    static lv_style_t top_pane_style;
    lv_style_init(&top_pane_style);
    lv_style_set_bg_color(&top_pane_style, lv_color_black());
    lv_style_set_border_width(&top_pane_style, 0);
    lv_style_set_radius(&top_pane_style, 0);

    lv_obj_t *top_pane = lv_obj_create(lv_scr_act());
    lv_obj_add_style(top_pane, &top_pane_style, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(top_pane, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(top_pane, BSP_LCD_H_RES, BSP_LCD_V_RES / 2);
    lv_obj_align_to(top_pane, lv_scr_act(), LV_ALIGN_TOP_MID, 0, 0);
    
    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, lv_color_white());
    lv_style_set_text_font(&label_style, &lv_font_montserrat_48);

    lv_obj_t *label = lv_label_create(top_pane);
    lv_obj_add_style(label, &label_style, LV_PART_MAIN);
    lv_label_set_text(label, "LiPo");
    lv_obj_align_to(label, top_pane, LV_ALIGN_CENTER, 0, 0);

    static lv_style_t label2_style;
    lv_style_init(&label2_style);
    lv_style_set_text_color(&label2_style, lv_color_black());
    lv_style_set_text_font(&label2_style, &lv_font_montserrat_28);

    lv_obj_t *label2 = lv_label_create(canvas);
    lv_obj_add_style(label2, &label2_style, LV_PART_MAIN);
    lv_obj_set_width(label2, BSP_LCD_H_RES);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_label_set_text(label2, "UBERFOO HEAVY INDUSTRIES");
    lv_obj_align_to(label2, canvas, LV_ALIGN_TOP_MID, 0, 40);

    bsp_display_unlock();
    
    xTaskCreate(demo_task, "metaballs", 4096, params, tskIDLE_PRIORITY + 1, NULL);

    // lv_obj_t *img = lv_gif_create(lv_scr_act());
    
    /* Set src of image with file name */
    // lv_gif_set_src(img, "S:/spiffs/lazer.gif");

    /* Align object */
    // lv_obj_center(img);


    return 0;
}
