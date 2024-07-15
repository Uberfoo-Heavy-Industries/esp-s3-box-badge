#include "MenuPage.h"
#include "NamePage.h"
#include "DemoSettingsPage.h"
#include "MainPage.h"

// Initialize the static member
MenuPage* MenuPage::instance = nullptr;

MenuPage::MenuPage(lv_obj_t *parent) : Page(parent) {
    // Initialize menu page content
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Menu Page");

    // Create menu buttons
    lv_obj_t *btn1 = lv_btn_create(page);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -30);
    lv_obj_t *btn1_label = lv_label_create(btn1);
    lv_label_set_text(btn1_label, "Name");
    uint8_t* btn1_option = new uint8_t(1);
    lv_obj_add_event_cb(btn1, options_btn_event_cb, LV_EVENT_CLICKED, btn1_option);

    lv_obj_t *btn2 = lv_btn_create(page);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 30);
    lv_obj_t *btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Demo Options");
    uint8_t* btn2_option = new uint8_t(2);
    lv_obj_add_event_cb(btn2, options_btn_event_cb, LV_EVENT_CLICKED, btn2_option);

    // Create a back button to return to the main page
    lv_obj_t *back_btn = lv_btn_create(page);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t *back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, this);
}

MenuPage* MenuPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MenuPage(parent);
    }
    return instance;
}

void MenuPage::show() {
    Page::show();
}

void MenuPage::options_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        uint8_t option = *static_cast<uint8_t*>(lv_event_get_user_data(e));
        MenuPage::getInstance()->hide();
        if (option == 1) {
            NamePage::getInstance()->show();
        } else if (option == 2) {
            DemoSettingsPage::getInstance()->show();
        }
    }
}

void MenuPage::back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        MenuPage::getInstance()->hide();
        MainPage::getInstance()->show();
    }
}

