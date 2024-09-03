#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "bsp/esp-bsp.h"
#include "bsp_board.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_random.h"
#include "lvgl.h"
#include "touch_element/touch_button.h"
#include "iot_button.h"

#include "main.h"
#include "metaballs.h"
#include "fire.h"
#include "Page.h"
#include "MainPage.h"
#include "MenuPage.h"
#include "NamePage.h"
#include "MessagePage.h"
#include "DemoSettingsPage.h"
#include "MessageLogPage.h"
#include "ESPNowService.h"
#include "MessageService.h"

void button_cb(void *button_handle, void *usr_data) {
    ESP_LOGI("button", "Button event.");
    Page::hideAll();
    MenuPage::getInstance()->show();
}

extern "C" int app_main()
{
    ESP_LOGD("main", "startup...");

    unsigned int seed = esp_random();
    srand(seed);
    
    ESP_LOGI("main", "random seed: %x", seed);

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
    
    /* Configure I2S peripheral and Power Amplifier */
    bsp_board_init();
    
    /* Lock display operations */
    bsp_display_lock(0);

    // Create a screen object
    lv_obj_t *scr = lv_scr_act();

    // Initialize pages
    MainPage::getInstance(scr);
    MenuPage::getInstance(scr);
    NamePage::getInstance(scr);
    MessagePage::getInstance(scr);
    DemoSettingsPage::getInstance(scr);
    MessageLogPage::getInstance(scr);

    bsp_display_unlock();
    
    // PersistenceService::getInstance()->setStateBits("demo_bits", DEFAULT_DEMO_STATE);
    uint32_t state = PersistenceService::getInstance()->getStateBits("demo_bits", DEFAULT_DEMO_STATE);
    ESP_LOGI("main", "State %08" PRIx32, state);

    // Load the main page initially
    MainPage::getInstance()->show();

    // ESPNowService::getInstance();
    // MessageService::getInstance();

    ESP_LOGI("main", "\tDescription\tInternal\tSPIRAM");
    ESP_LOGI("main", "Current Free Memory\t%d\t\t%d",
                heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI("main", "Min. Ever Free Size\t%d\t\t%d",
                heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));

    return 0;
}
