#ifndef _DEMO_SETTINGS_PAGE_H_
#define _DEMO_SETTINGS_PAGE_h_

#include "Page.h"

class DemoSettingsPage : public Page {
public:
    static DemoSettingsPage* getInstance(lv_obj_t *parent = nullptr);
    void show() override;

private:
    DemoSettingsPage(lv_obj_t *parent);
    static void back_btn_event_cb(lv_event_t *e);
    static void save_btn_event_cb(lv_event_t *e);
    static void switch_event_cb(lv_event_t *e);

    lv_obj_t *checkbox1;
    lv_obj_t *checkbox2;
    lv_obj_t *checkbox3;
    lv_obj_t *checkbox_container;
    lv_obj_t *enable_switch;
    lv_obj_t *save_btn;
    lv_obj_t *back_btn;

    static DemoSettingsPage *instance;
};

#endif // _DEMO_SETTINGS_PAGE_H_
