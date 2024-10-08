#include "bsp/esp-bsp.h"
#include "Page.h"

// Initialize the static member
std::vector<Page*> Page::pages;

Page::Page(lv_obj_t *parent) {
    pages.push_back(this);
    page = lv_obj_create(parent);
    lv_obj_set_size(page, BSP_LCD_H_RES, LV_VER_RES); // Set the size of the page to fill the screen
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
}

void Page::show() {
    bsp_display_lock(0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
    bsp_display_unlock();
}

void Page::hide() {
    bsp_display_lock(0);
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
    bsp_display_unlock();
}

bool Page::is_active() {
    return !lv_obj_has_flag(page, LV_OBJ_FLAG_HIDDEN);
}

void Page::hideAll() {
    for (Page *page : pages) {
        page->hide();
    }
}
