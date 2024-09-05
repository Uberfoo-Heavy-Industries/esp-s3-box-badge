#include <stdio.h>
#include "TextEntryPage.h"
#include "MenuPage.h"
#include "PersistenceService.h"
#include "ESPNowService.h"

TextEntryPage::TextEntryPage(lv_obj_t *parent, const char *actionText) : Page(parent) {
    bsp_display_lock(0);

    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(page, 0, LV_PART_MAIN);

    // Create a container for textbox and button
    container = lv_obj_create(page);
    lv_obj_set_size(container, 320, 60);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 0);

    // Set the padding to 0 for all sides
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);

    // Set flex layout for the checkbox container
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);

    // Create a text area
    text_area = lv_textarea_create(container);
    lv_obj_set_size(text_area, 230, 40);

    // Create a send button
    action_btn = lv_btn_create(container);
    lv_obj_set_size(action_btn, 58, 40);
    lv_obj_t *send_btn_label = lv_label_create(action_btn);
    lv_obj_set_style_pad_all(send_btn_label, 0, LV_PART_MAIN);
    lv_obj_center(send_btn_label);
    lv_label_set_text(send_btn_label, actionText);
    lv_obj_add_event_cb(action_btn, send_btn_event_cb, LV_EVENT_CLICKED, this);


    // Create a keyboard
    keyboard = lv_keyboard_create(page);
    lv_keyboard_set_textarea(keyboard, text_area);
    lv_obj_align(keyboard, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_size(keyboard, 320, 166);  // Adjust size to fit within the screen

    // Create a back button to return to the menu page
    bsp_display_unlock();
}

void TextEntryPage::show() {
    bsp_display_lock(0);

    const char *name = PersistenceService::getInstance()->getName();
    lv_textarea_set_text(text_area, name);
    Page::show();

    bsp_display_unlock();
}

void TextEntryPage::send_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    TextEntryPage *page = (TextEntryPage*)lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_add_state(e->target, LV_STATE_DISABLED);
        page->performAction();
        lv_obj_clear_state(e->target, LV_STATE_DISABLED);
    }
}
