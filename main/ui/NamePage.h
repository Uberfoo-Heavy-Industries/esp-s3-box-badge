#ifndef _NAME_PAGE_H_
#define _NAME_PAGE_H_

#include "Page.h"

class NamePage : public Page {
public:
    static NamePage* getInstance(lv_obj_t *parent = nullptr);
    void show() override;

private:
    NamePage(lv_obj_t *parent);
    static void back_btn_event_cb(lv_event_t *e);
    static void save_btn_event_cb(lv_event_t *e);

    lv_obj_t *text_area;
    lv_obj_t *keyboard;
    lv_obj_t *save_btn;
    lv_obj_t *back_btn;

    static NamePage *instance;
};

#endif // _NAME_PAGE_H_
