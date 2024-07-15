#include <stdio.h>

#include "DemoSettingsPage.h"
#include "MenuPage.h"

DemoSettingsPage* DemoSettingsPage::instance = nullptr;

DemoSettingsPage::DemoSettingsPage(lv_obj_t *parent) : Page(parent) {
    // Initialize options page 2 content
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Options Page 2");

    // Create an enable switch
    enable_switch = lv_switch_create(page);
    lv_obj_align(enable_switch, LV_ALIGN_CENTER, 0, -60);
    lv_obj_add_event_cb(enable_switch, switch_event_cb, LV_EVENT_VALUE_CHANGED, this);

    // Create a container for checkboxes
    checkbox_container = lv_obj_create(page);
    lv_obj_set_size(checkbox_container, 200, 100);
    lv_obj_align(checkbox_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(checkbox_container, LV_OBJ_FLAG_HIDDEN);

    // Set flex layout for the checkbox container
    lv_obj_set_flex_flow(checkbox_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(checkbox_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Create checkboxes
    checkbox1 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox1, "Option 1");

    checkbox2 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox2, "Option 2");

    checkbox3 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox3, "Option 3");

    // Create a save button
    save_btn = lv_btn_create(page);
    lv_obj_set_size(save_btn, 100, 30);
    lv_obj_align(save_btn, LV_ALIGN_BOTTOM_LEFT, 10, 0);
    lv_obj_t *save_btn_label = lv_label_create(save_btn);
    lv_label_set_text(save_btn_label, "Save");
    lv_obj_add_event_cb(save_btn, save_btn_event_cb, LV_EVENT_CLICKED, this);

    // Create a back button to return to the menu page
    back_btn = lv_btn_create(page);
    lv_obj_set_size(back_btn, 50, 30);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_RIGHT, -10, 0);
    lv_obj_t *back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, this);
}

DemoSettingsPage* DemoSettingsPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new DemoSettingsPage(parent);
    }
    return instance;
}

void DemoSettingsPage::show() {
    Page::show();
}

void DemoSettingsPage::back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        DemoSettingsPage::getInstance()->hide();
        MenuPage::getInstance()->show();
    }
}

void DemoSettingsPage::save_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Handle save action
        DemoSettingsPage* optionsPage = static_cast<DemoSettingsPage*>(lv_event_get_user_data(e));
        bool option1 = lv_obj_has_state(optionsPage->checkbox1, LV_STATE_CHECKED);
        bool option2 = lv_obj_has_state(optionsPage->checkbox2, LV_STATE_CHECKED);
        bool option3 = lv_obj_has_state(optionsPage->checkbox3, LV_STATE_CHECKED);
        printf("Option 1: %s\n", option1 ? "Checked" : "Unchecked");
        printf("Option 2: %s\n", option2 ? "Checked" : "Unchecked");
        printf("Option 3: %s\n", option3 ? "Checked" : "Unchecked");
    }
}

void DemoSettingsPage::switch_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        DemoSettingsPage* optionsPage = static_cast<DemoSettingsPage*>(lv_event_get_user_data(e));
        lv_obj_t *switch_obj = lv_event_get_target(e);
        bool checked = lv_obj_has_state(switch_obj, LV_STATE_CHECKED);
        if (checked) {
            lv_obj_clear_flag(optionsPage->checkbox_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(optionsPage->checkbox_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
}
