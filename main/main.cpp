#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "bsp/esp-bsp.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "lvgl.h"
#include "touch_element/touch_button.h"
#include "iot_button.h"

#include "main.h"
#include "metaballs.h"
#include "fire.h"
#include "MainPage.h"
#include "MenuPage.h"
#include "NamePage.h"
#include "DemoSettingsPage.h"

void button_cb(void *button_handle, void *usr_data) {
    bsp_display_lock(0);
    ESP_LOGI("button", "Button event.");
    if (MainPage::getInstance()->is_active()) {
        MainPage::getInstance()->hide();
    }
    MenuPage::getInstance()->show();
    bsp_display_unlock();
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

    button_handle_t buttons[BSP_BUTTON_NUM];

    bsp_iot_button_create(buttons, NULL, BSP_BUTTON_NUM);
    iot_button_register_cb(buttons[BSP_BUTTON_MAIN], BUTTON_PRESS_DOWN, button_cb, NULL); 

     /* Mount SPIFFS */
    bsp_spiffs_mount();

    bsp_display_lock(0);

        // Create a screen object
    lv_obj_t *scr = lv_scr_act();

    // Initialize pages
    MainPage::getInstance(scr);
    MenuPage::getInstance(scr);
    NamePage::getInstance(scr);
    DemoSettingsPage::getInstance(scr);

    // Load the main page initially
    MainPage::getInstance()->show();

    bsp_display_unlock();

    ESP_LOGI("main", "\tDescription\tInternal\tSPIRAM");
    ESP_LOGI("main", "Current Free Memory\t%d\t\t%d",
                heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI("main", "Min. Ever Free Size\t%d\t\t%d",
                heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));

    return 0;
}
