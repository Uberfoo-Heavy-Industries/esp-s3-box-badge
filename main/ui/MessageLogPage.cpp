#include "MessageLogPage.h"

MessageLogPage* MessageLogPage::instance = nullptr;

MessageLogPage::MessageLogPage(lv_obj_t *parent) : Page(parent) {
   
    list = lv_table_create(page);
    
    messageService = MessageService::getInstance();
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);
    lv_table_set_col_cnt(list, 1);
    lv_table_set_row_cnt(list, 1); // Ensure there is at least one row

    // Get the width of the parent object (e.g., screen or container)
    lv_coord_t parent_width = lv_obj_get_width(list) - 10;
    
    // Set the column width to the parent's width
    lv_table_set_col_width(list, 0, 280);

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
    bsp_display_lock(0);
    Page::show();
    bsp_display_unlock();
}

void MessageLogPage::addMessage(const char *username, const char *text) {
    
    bsp_display_lock(0);

    uint16_t row = lv_table_get_row_cnt(list);
    ESP_LOGI("MessageLogPage", "adding message: %s: %s", username, text);
    // If the table is in the "empty" state, overwrite the placeholder row
    if (row == 1 && strcmp(lv_table_get_cell_value(list, 0, 0), "") == 0) {
        lv_table_set_cell_value_fmt(list, 0, 0, "%s: %s", username, text);
    } else {
        // Otherwise, add a new row
        lv_table_set_row_cnt(list, row + 1);
        lv_table_set_cell_value_fmt(list, row, 0, "%s: %s", username, text);
    }
    ESP_LOGI("MessageLogPage", "message added: %s: %s", username, text);

    lv_coord_t y = lv_obj_get_self_height(list);

	//If the object content is big enough to scroll
	if (y > lv_obj_get_height(list))
	{
		//Calculate the "out of view" y size
		lv_coord_t outOfView = y - lv_obj_get_height(list);
		lv_coord_t currScrollPos = lv_obj_get_scroll_y(list);

		if(outOfView > currScrollPos)
		{
			//Calculate the difference between the required scroll pos and the current scroll pos
			lv_coord_t differenceToScroll = - (outOfView - currScrollPos);

			//this will bring the bottom of the table into view
			lv_obj_scroll_by(list, 0, differenceToScroll, LV_ANIM_ON);
		}
	}

    lv_obj_invalidate(list);

    bsp_display_unlock();
}