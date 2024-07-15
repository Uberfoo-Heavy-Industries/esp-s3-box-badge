#ifndef MENUPAGE_H
#define MENUPAGE_H

#include "Page.h"

class MenuPage : public Page {
public:
    static MenuPage* getInstance(lv_obj_t *parent = nullptr);
    void show() override;

private:
    MenuPage(lv_obj_t *parent);
    static void options_btn_event_cb(lv_event_t *e);
    static void back_btn_event_cb(lv_event_t *e);

    static MenuPage *instance;
};

#endif // MENUPAGE_H

