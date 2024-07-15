#ifndef _MAINPAGE_H_
#define _MAINPAGE_H_

#include "Page.h"

class MainPage : public Page {
public:
    static MainPage* getInstance(lv_obj_t *parent = nullptr);
    void show() override;

private:
    MainPage(lv_obj_t *parent);
    static void menu_btn_event_cb(lv_event_t *e);

    static MainPage *instance;
};

#endif // _MAINPAGE_H_

