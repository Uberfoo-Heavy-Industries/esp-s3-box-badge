#include "esp_log.h"

#include "MainPage.h"
#include "MenuPage.h"

// Initialize the static member
MainPage* MainPage::instance = nullptr;

MainPage::MainPage(lv_obj_t *parent) : Page(parent) {
    // Initialize main page content
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Main Page");

    // Create a button to go to the menu page
    lv_obj_t *btn = lv_btn_create(page);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Go to Menu");
    lv_obj_add_event_cb(btn, menu_btn_event_cb, LV_EVENT_CLICKED, this);
}

MainPage* MainPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MainPage(parent);
    }
    return instance;
}

void MainPage::show() {
    Page::show();
}

void MainPage::menu_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    MainPage *instance = static_cast<MainPage*>(lv_event_get_user_data(e));
    if (code == LV_EVENT_CLICKED) {
        instance->hide();
        MenuPage::getInstance()->show();
    }
}
