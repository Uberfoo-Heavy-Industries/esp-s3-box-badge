#include <stdio.h>

#include "DemoSettingsPage.h"
#include "MenuPage.h"
#include "MainPage.h"
#include "PersistenceService.h"
#include "demo.h"

DemoSettingsPage* DemoSettingsPage::instance = nullptr;

DemoSettingsPage::DemoSettingsPage(lv_obj_t *parent) : Page(parent) {
    // Initialize options page 2 content
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Demo Settings");

    // Create an enable switch
    enable_switch = lv_switch_create(page);
    lv_obj_align(enable_switch, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_event_cb(enable_switch, switch_event_cb, LV_EVENT_VALUE_CHANGED, this);

    // Create a container for checkboxes
    checkbox_container = lv_obj_create(page);
    lv_obj_set_size(checkbox_container, 294, 115);
    lv_obj_align(checkbox_container, LV_ALIGN_CENTER, 0, 7);
    lv_obj_add_flag(checkbox_container, LV_OBJ_FLAG_HIDDEN);

    // Set flex layout for the checkbox container
    lv_obj_set_flex_flow(checkbox_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(checkbox_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Create checkboxes
    checkbox1 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox1, "Fire");

    checkbox2 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox2, "Metaballs");

    checkbox3 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox3, "Deform");

    checkbox4 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox4, "Rotozoom");

    checkbox5 = lv_checkbox_create(checkbox_container);
    lv_checkbox_set_text(checkbox5, "Plasma");

    // Create a save button
    save_btn = lv_btn_create(page);
    lv_obj_set_size(save_btn, 65, 35);
    lv_obj_align(save_btn, LV_ALIGN_BOTTOM_LEFT, 10, 0);
    lv_obj_t *save_btn_label = lv_label_create(save_btn);
    lv_label_set_text(save_btn_label, "Save");
    lv_obj_add_event_cb(save_btn, save_btn_event_cb, LV_EVENT_CLICKED, this);

    // Create a back button to return to the menu page
    back_btn = lv_btn_create(page);
    lv_obj_set_size(back_btn, 65, 35);
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
    uint32_t bitmap = PersistenceService::getInstance()->getStateBits("demo_bits", DEFAULT_DEMO_STATE);

    if (bitmap == 0) {
        lv_obj_clear_state(enable_switch, LV_STATE_CHECKED);
        lv_obj_add_flag(checkbox_container, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_state(enable_switch, LV_STATE_CHECKED);
        lv_obj_clear_flag(checkbox_container, LV_OBJ_FLAG_HIDDEN);

        if (bitmap & DEMO_FIRE) {lv_obj_add_state(checkbox1, LV_STATE_CHECKED);} 
        else {lv_obj_clear_state(checkbox1, LV_STATE_CHECKED);}
        if (bitmap & DEMO_METABALLS) {lv_obj_add_state(checkbox2, LV_STATE_CHECKED);}
        else {lv_obj_clear_state(checkbox2, LV_STATE_CHECKED);}
        if (bitmap & DEMO_DEFORM) {lv_obj_add_state(checkbox3, LV_STATE_CHECKED);}
        else {lv_obj_clear_state(checkbox3, LV_STATE_CHECKED);}
        if (bitmap & DEMO_ROTOZOOM) {lv_obj_add_state(checkbox4, LV_STATE_CHECKED);}
        else {lv_obj_clear_state(checkbox4, LV_STATE_CHECKED);}
        if (bitmap & DEMO_PLASMA) {lv_obj_add_state(checkbox5, LV_STATE_CHECKED);}
        else {lv_obj_clear_state(checkbox5, LV_STATE_CHECKED);}
    }

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
    ESP_LOGI("DemoSettingsPage::save_btn_event_cb", "save");
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {

        // Check if all demos are turned off
        if (!lv_obj_has_state(DemoSettingsPage::getInstance()->enable_switch, LV_STATE_CHECKED)) {
            auto persistencSvc =  PersistenceService::getInstance();
            persistencSvc->setStateBits("demo_bits", 0);
            return;          
        }

        // Handle save action
        DemoSettingsPage* optionsPage = static_cast<DemoSettingsPage*>(lv_event_get_user_data(e));
        const bool option1 = lv_obj_has_state(optionsPage->checkbox1, LV_STATE_CHECKED);
        const bool option2 = lv_obj_has_state(optionsPage->checkbox2, LV_STATE_CHECKED);
        const bool option3 = lv_obj_has_state(optionsPage->checkbox3, LV_STATE_CHECKED);
        const bool option4 = lv_obj_has_state(optionsPage->checkbox4, LV_STATE_CHECKED);
        const bool option5 = lv_obj_has_state(optionsPage->checkbox5, LV_STATE_CHECKED);

        uint32_t bitmap = 0;
        if (option1) bitmap |= DEMO_FIRE;
        if (option2) bitmap |= DEMO_METABALLS;
        if (option3) bitmap |= DEMO_DEFORM;
        if (option4) bitmap |= DEMO_ROTOZOOM;
        if (option5) bitmap |= DEMO_PLASMA;

        ESP_LOGI("DemoSettingsPage::save_btn_event_cb", "Saving demo bits: %08" PRIx32, bitmap);
        auto persistencSvc =  PersistenceService::getInstance();
        persistencSvc->setStateBits("demo_bits", bitmap);
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
