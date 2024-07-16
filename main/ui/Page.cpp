#include "bsp/esp-bsp.h"
#include "Page.h"

Page::Page(lv_obj_t *parent) {
    page = lv_obj_create(parent);
    lv_obj_set_size(page, BSP_LCD_H_RES, LV_VER_RES); // Set the size of the page to fill the screen
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
}

void Page::show() {
    lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
}

void Page::hide() {
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
}

bool Page::is_active() {
    return !lv_obj_has_flag(page, LV_OBJ_FLAG_HIDDEN);
}
