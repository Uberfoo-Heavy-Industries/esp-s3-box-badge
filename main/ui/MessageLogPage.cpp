#include "MessageLogPage.h"

MessageLogPage* MessageLogPage::instance = nullptr;

MessageLogPage::MessageLogPage(lv_obj_t *parent) : Page(parent) {
    static lv_style_t style_list_item;
    lv_style_init(&style_list_item);
    lv_style_set_height(&style_list_item, 60); // Set the height of the list item
    

    messageService = MessageService::getInstance();
    list = lv_table_create(page);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);
    lv_table_set_col_cnt(list, 1);
    lv_table_set_row_cnt(list, 1); // Ensure there is at least one row
    //lv_table_set_cell_value(list, 0, 0, ""); // Set the first cell to empty
    lv_obj_remove_style(list, NULL, LV_PART_ITEMS | LV_STATE_PRESSED);

    messageService->registerCallback(std::bind(&MessageLogPage::addMessage, this, std::placeholders::_1, std::placeholders::_2));
}

MessageLogPage* MessageLogPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MessageLogPage(parent);
    }
    return instance;
}

void MessageLogPage::show() {
    Page::show();
}

void MessageLogPage::addMessage(const char *username, const char *text) {
    
    
    
    // messages.push_back(message);

    uint16_t row = lv_table_get_row_cnt(list);
    
    ESP_LOGI("MessageLogPage", "adding message: %s: %s", username, text);

    bsp_display_lock(0);
    // If the table is in the "empty" state, overwrite the placeholder row
    if (row == 1 && strcmp(lv_table_get_cell_value(list, 0, 0), "") == 0) {
        lv_table_set_cell_value_fmt(list, 0, 0, "%s: %s", username, text);
    } else {
        // Otherwise, add a new row
        lv_table_set_row_cnt(list, row + 1);
        lv_table_set_cell_value_fmt(list, row, 0, "%s: %s", username, text);
    }


    // Scroll to the bottom of the table
    lv_coord_t table_height = lv_obj_get_height(list);
    lv_coord_t content_height = lv_table_get_row_cnt(list) * lv_obj_get_style_text_line_space(list, LV_PART_ITEMS);
    // lv_obj_scroll_to_y(list, content_height - table_height, LV_ANIM_OFF);

    // lv_obj_invalidate(page);
    lv_obj_invalidate(list);

    bsp_display_unlock();
}