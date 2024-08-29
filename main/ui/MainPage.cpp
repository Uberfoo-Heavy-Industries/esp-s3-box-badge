#include "bsp/esp-bsp.h"
#include "esp_log.h"

#include "MainPage.h"
#include "MenuPage.h"
#include "PersistenceService.h"
#include "metaballs.h"
#include "fire.h"
#include "plasma.h"
#include "deform.h"
#include "rotozoom.h"

// Initialize the static member
MainPage* MainPage::instance = nullptr;

MainPage::MainPage(lv_obj_t *parent) : Page(parent) {
    state = PersistenceService::getInstance()->getStateBits("demos", DEFAULT_DEMO_STATE);

    lock = xSemaphoreCreateMutex();

    ESP_LOGI("MainPage::MainPage", "state: %lx", state);

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
    
    lv_obj_t *img = lv_img_create(top_pane);
    lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    lv_img_set_src(img, "S:/spiffs/speech-balloon.png");
    lv_obj_set_size(img, 25, 25);
    lv_obj_align(img, LV_ALIGN_TOP_RIGHT, 0, 0);
    
    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, lv_color_white());
    lv_style_set_text_font(&label_style, &lv_font_montserrat_48);

    label = lv_label_create(top_pane);
    lv_obj_add_style(label, &label_style, LV_PART_MAIN);
    lv_label_set_text(label, PersistenceService::getInstance()->getName());
    lv_obj_align_to(label, top_pane, LV_ALIGN_CENTER, 0, 0);

    static lv_style_t label2_style;
    lv_style_init(&label2_style);
    lv_style_set_text_color(&label2_style, lv_color_black());
    lv_style_set_text_font(&label2_style, &lv_font_montserrat_28);

    // lv_obj_t *label2 = lv_label_create(canvas);
    // lv_obj_add_style(label2, &label2_style, LV_PART_MAIN);
    // lv_obj_set_width(label2, BSP_LCD_H_RES);
    // lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    // lv_label_set_text(label2, "UBERFOO HEAVY INDUSTRIES");
    // lv_obj_align_to(label2, canvas, LV_ALIGN_TOP_MID, 0, 40);

    currentDemo = nullptr;
    if (state != 0) loadDemo();

    timer_handle = xTimerCreate("demo_timer", DEMO_TIMEOUT, pdTRUE, nullptr, demo_timer_cb);
}

MainPage* MainPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MainPage(parent);
    }
    return instance;
}

void MainPage::show() {
    state = PersistenceService::getInstance()->getStateBits("demos", DEFAULT_DEMO_STATE);

    if (task_handle == nullptr) {
        xTaskCreate(demo_task, "demo", 1024 * 2, this, tskIDLE_PRIORITY + 1, &task_handle);
    } else {
        vTaskResume(task_handle);
    }
    xTimerStart(timer_handle, portMAX_DELAY);

    // Update text
    lv_label_set_text(label, PersistenceService::getInstance()->getName());
    lv_obj_align_to(label, top_pane, LV_ALIGN_CENTER, 0, 0);

    Page::show();
}

void MainPage::hide() {
    vTaskSuspend(task_handle);
    xTimerStop(timer_handle, portMAX_DELAY);
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

    MainPage *page = MainPage::getInstance();
    while (true) {
        xSemaphoreTake(page->lock, portMAX_DELAY);
        ESP_LOGD("MainPage::demo_task", "render frame");
        if (page->currentDemo) {
            page->currentDemo->renderFrame();
        }
        xSemaphoreGive(page->lock);
        vPortYield();
    }
}

void MainPage::load_task(void *obj) {
    MainPage *page = MainPage::getInstance();
    ESP_LOGI("MainPage::load_task", "getting lock");
    xSemaphoreTake(page->lock, portMAX_DELAY);
    ESP_LOGI("MainPage::load_task", "got lock");
    page->getNextIndex();
    ESP_LOGI("MainPage::load_task", "loading demo");
    page->loadDemo();
    xSemaphoreGive(page->lock);
    vTaskDelete(NULL);
}

void MainPage::demo_timer_cb(TimerHandle_t handle) {
    MainPage *page = MainPage::getInstance();
    ESP_LOGI("MainPage::demo_timer_cb", "loading next demo");
    BaseType_t ret = xTaskCreate(load_task, "load", 1024 * 3, page, tskIDLE_PRIORITY + 2, &page->task_handle2);
    if (ret != pdPASS) {
        ESP_LOGE("MainPage::demo_timer_cb", "Error launching demp load task (%i)", ret);
    }
}

void MainPage::getNextIndex() {
    ++index %= DEMO_COUNT;
    int i = 0;
    while (!((1 << index) & state) && i < DEMO_COUNT) {
        ESP_LOGI("MainPage::getNextIndex", "index = %i, state = %lx", index, state);
        ++index %= DEMO_COUNT;
        i++;
    }
    if (i == DEMO_COUNT) {
        index = -1;
    }
}

void MainPage::loadDemo() {
    if (currentDemo != nullptr) {
        ESP_LOGI("MainPage::loadDemo", "freeing demo");
        delete currentDemo;
    }

    ESP_LOGI("MainPage::loadDemo", "\tDescription\tInternal\tSPIRAM");
    ESP_LOGI("MainPage::loadDemo", "Current Free Memory\t%d\t\t%d",
                heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI("MainPage::loadDemo", "Min. Ever Free Size\t%d\t\t%d",
                heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
                heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI("MainPage::loadDemo", "Largest Block Size\t%d\t\t%d",
                heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
                heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));

    ESP_LOGI("MainPage::loadDemo", "loading demo #%i", index);
    switch (index) {
        case 0:
            // demoBuf = (char *)heap_caps_malloc(sizeof(Fire), MALLOC_CAP_SPIRAM);
            // if (!demoBuf) {
            //     ESP_LOGE("MainPage", "Could not allocate Fire demo");
            //     esp_restart();
            // }
            currentDemo = new Fire(canvas, params.width, params.height);
            break;
            
        case 1:
            // demoBuf = (char *)heap_caps_malloc(sizeof(Metaballs), MALLOC_CAP_SPIRAM);
            // if (!demoBuf) {
            //     ESP_LOGE("MainPage", "Could not allocate Metaballs demo");
            //     esp_restart();
            // }
            currentDemo = new Metaballs(canvas, params.width, params.height);
            break;
            
        case 2:
            // demoBuf = (char *)heap_caps_malloc(sizeof(Rotozoom), MALLOC_CAP_SPIRAM);
            // if (!demoBuf) {
            //     ESP_LOGE("MainPage", "Could not allocate Rotozoom demo");
            //     esp_restart();
            // }
            currentDemo = new Deform(canvas, params.width, params.height);
            break;
            
        case 3:
            // demoBuf = (char *)heap_caps_malloc(sizeof(Deform), MALLOC_CAP_SPIRAM);
            // if (!demoBuf) {
            //     ESP_LOGE("MainPage", "Could not allocate Deform demo");
            //     esp_restart();
            // }
            currentDemo = new Rotozoom(canvas, params.width, params.height);
            break;

        default:
            currentDemo = nullptr;  
    }

    ESP_LOGI("MainPage::loadDemo", "demo loaded");
}