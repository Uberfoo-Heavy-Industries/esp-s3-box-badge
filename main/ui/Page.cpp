#include "Page.h"

Page::Page(lv_obj_t *parent) {
    page = lv_obj_create(parent);
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
}

void Page::show() {
    lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
}

void Page::hide() {
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
}
