#include <stdio.h>
#include "MessagePage.h"
#include "MenuPage.h"
#include "SettingsService.h"
#include "ESPNowService.h"

MessagePage* MessagePage::instance = nullptr;

MessagePage::MessagePage(lv_obj_t *parent) : Page(parent) {
    // Create a text area
    text_area = lv_textarea_create(page);
    lv_obj_set_size(text_area, 300, 40);
    lv_obj_align(text_area, LV_ALIGN_TOP_MID, 0, 5);

    // Create a keyboard
    keyboard = lv_keyboard_create(page);
    lv_keyboard_set_textarea(keyboard, text_area);
    lv_obj_align(keyboard, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_size(keyboard, 320, 120);  // Adjust size to fit within the screen

    // Create a save button
    send_btn = lv_btn_create(page);
    lv_obj_set_size(send_btn, 60, 30);
    lv_obj_align(send_btn, LV_ALIGN_BOTTOM_LEFT, 10, 0);
    lv_obj_t *send_btn_label = lv_label_create(send_btn);
    lv_label_set_text(send_btn_label, "Send");
    lv_obj_add_event_cb(send_btn, send_btn_event_cb, LV_EVENT_CLICKED, this);

    // Create a back button to return to the menu page
    back_btn = lv_btn_create(page);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_RIGHT, -10, 0);
    lv_obj_t *back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, this);
}

MessagePage* MessagePage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MessagePage(parent);
    }
    return instance;
}

void MessagePage::show() {
    const char *name = SettingsService::getInstance()->getName();
    lv_textarea_set_text(text_area, name);
    Page::show();
}

void MessagePage::back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        MessagePage::getInstance()->hide();
        MenuPage::getInstance()->show();
    }
}

void MessagePage::send_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Handle save action
        const char *text = lv_textarea_get_text(MessagePage::getInstance()->text_area);
        // Save the text or perform any other action
        ESP_LOGD("MessagePage", "Sending text: %s", text);

        ESPNowService::getInstance()->sendMessage(text);
    }
}
