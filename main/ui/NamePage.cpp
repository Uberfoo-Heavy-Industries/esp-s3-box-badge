#include <stdio.h>
#include "NamePage.h"
#include "MenuPage.h"

NamePage* NamePage::instance = nullptr;

NamePage::NamePage(lv_obj_t *parent) : Page(parent) {
    // Initialize options page 1 content
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Options Page 1");

    // Create a text area
    text_area = lv_textarea_create(page);
    lv_obj_set_size(text_area, 200, 50);
    lv_obj_align(text_area, LV_ALIGN_CENTER, 0, -40);

    // Create a keyboard
    keyboard = lv_keyboard_create(page);
    lv_keyboard_set_textarea(keyboard, text_area);
    lv_obj_align(keyboard, LV_ALIGN_CENTER, 0, 40);

    // Create a save button
    lv_obj_t *save_btn = lv_btn_create(page);
    lv_obj_align(save_btn, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_t *save_btn_label = lv_label_create(save_btn);
    lv_label_set_text(save_btn_label, "Save");
    lv_obj_add_event_cb(save_btn, save_btn_event_cb, LV_EVENT_CLICKED, this);

    // Create a back button to return to the menu page
    lv_obj_t *back_btn = lv_btn_create(page);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t *back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, this);
}

NamePage* NamePage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new NamePage(parent);
    }
    return instance;
}

void NamePage::show() {
    Page::show();
}

void NamePage::back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        NamePage::getInstance()->hide();
        MenuPage::getInstance()->show();
    }
}

void NamePage::save_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Handle save action
        const char *text = lv_textarea_get_text(NamePage::getInstance()->text_area);
        // Save the text or perform any other action
        printf("Saved text: %s\n", text);
    }
}
