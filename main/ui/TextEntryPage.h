#ifndef _TEXT_ENTRY_PAGE_H_
#define _TEXT_ENTRY_PAGE_H_

#include "Page.h"
#include "message.h"

class TextEntryPage : public Page {

protected:
    static TextEntryPage *getInstance(lv_obj_t *parent);
    TextEntryPage(lv_obj_t *parent, const char *actionText);
    lv_obj_t *text_area;

private:
    static void send_btn_event_cb(lv_event_t *e);

    virtual void performAction() = 0;

    lv_obj_t *container;
    lv_obj_t *keyboard;
    lv_obj_t *action_btn;

};

#endif // _TEXT_ENTRY_PAGE_H_
