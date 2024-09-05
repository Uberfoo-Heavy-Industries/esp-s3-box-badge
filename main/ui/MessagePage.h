#ifndef _MESSAGE_PAGE_H_
#define _MESSAGE_PAGE_H_

#include "TextEntryPage.h"

class MessagePage : public TextEntryPage {

public:
    static MessagePage *getInstance(lv_obj_t *parent = nullptr);

private:
    MessagePage(lv_obj_t *parent);
    void performAction();

    static MessagePage *instance;

};

#endif // _MESSAGE_PAGE_H_