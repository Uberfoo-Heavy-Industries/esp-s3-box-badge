#ifndef _MESSAGE_PAGE_H_
#define _MESSAGE_PAGE_H_

#include "Page.h"
#include "message.h"

class MessagePage : public Page {
public:
    static MessagePage* getInstance(lv_obj_t *parent = nullptr);
    void show() override;

private:
    MessagePage(lv_obj_t *parent);
    static void back_btn_event_cb(lv_event_t *e);
    static void send_btn_event_cb(lv_event_t *e);

    lv_obj_t *text_area;
    lv_obj_t *keyboard;
    lv_obj_t *send_btn;
    lv_obj_t *back_btn;

    static MessagePage *instance;
};

#endif // _MESSAGE_PAGE_H_
