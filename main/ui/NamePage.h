#ifndef _NAME_PAGE_H_
#define _NAME_PAGE_H_

#include "TextEntryPage.h"

class NamePage : public TextEntryPage {
public:
    static NamePage* getInstance(lv_obj_t *parent = nullptr);
    void show() override;

private:
    NamePage(lv_obj_t *parent);
    void performAction();

    static NamePage *instance;
};

#endif // _NAME_PAGE_H_
