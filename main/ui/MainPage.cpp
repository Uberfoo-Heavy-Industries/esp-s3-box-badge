#include "bsp/esp-bsp.h"
#include "esp_log.h"

#include "MainPage.h"
#include "MenuPage.h"
#include "SettingsService.h"
#include "metaballs.h"
#include "fire.h"

// Initialize the static member
MainPage* MainPage::instance = nullptr;

MainPage::MainPage(lv_obj_t *parent) : Page(parent) {
    buf = (lv_color_t *)heap_caps_malloc(LV_CANVAS_BUF_SIZE_TRUE_COLOR(BSP_LCD_H_RES, BSP_LCD_V_RES / 2), MALLOC_CAP_DEFAULT);

    canvas = lv_canvas_create(parent);
    lv_canvas_set_buffer(canvas, buf, BSP_LCD_H_RES, BSP_LCD_V_RES / 2, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align_to(canvas, parent, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_canvas_fill_bg(canvas, lv_color_make(0, 255, 0), LV_OPA_COVER);

    params = {
        .canvas = canvas,
        .width = BSP_LCD_H_RES,
        .height = BSP_LCD_V_RES / 2
    };

    static lv_style_t top_pane_style;
    lv_style_init(&top_pane_style);
    lv_style_set_bg_color(&top_pane_style, lv_color_black());
    lv_style_set_border_width(&top_pane_style, 0);
    lv_style_set_radius(&top_pane_style, 0);

    top_pane = lv_obj_create(parent);
    lv_obj_add_style(top_pane, &top_pane_style, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(top_pane, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(top_pane, BSP_LCD_H_RES, BSP_LCD_V_RES / 2);
    lv_obj_align_to(top_pane, parent, LV_ALIGN_TOP_MID, 0, 0);
    
    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, lv_color_white());
    lv_style_set_text_font(&label_style, &lv_font_montserrat_48);

    label = lv_label_create(top_pane);
    lv_obj_add_style(label, &label_style, LV_PART_MAIN);
    lv_label_set_text(label, SettingsService::getInstance()->getName());
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

}

MainPage* MainPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MainPage(parent);
    }
    return instance;
}

void MainPage::show() {
    if (task_handle == nullptr) {
        xTaskCreate(demo_task, "demo", 4096, &params, tskIDLE_PRIORITY + 1, &task_handle);
    } else {
        vTaskResume(task_handle);
    }

    // Update text
    lv_label_set_text(label, SettingsService::getInstance()->getName());
    lv_obj_align_to(label, top_pane, LV_ALIGN_CENTER, 0, 0);

    Page::show();
}

void MainPage::hide() {
    vTaskSuspend(task_handle);
    Page::hide();
}

void MainPage::menu_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    MainPage *instance = static_cast<MainPage*>(lv_event_get_user_data(e));
    if (code == LV_EVENT_CLICKED) {
        instance->hide();
        MenuPage::getInstance()->show();
    }
}

void MainPage::demo_task(void *obj) {

    demo_task_params *params = (demo_task_params *)obj;
    ESP_LOGD("demo_task", "dimensions: (%d, %d)", params->width, params->height);
    Demo *demo = new Fire(params->canvas, params->width, params->height);
    while (true) {
        bsp_display_lock(0);
        demo->renderFrame();
        bsp_display_unlock();
    }
}

