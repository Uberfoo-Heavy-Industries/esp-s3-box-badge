#ifndef PAGE_H
#define PAGE_H

#include "lvgl.h"

class Page {
public:
    Page(lv_obj_t *parent);
    virtual ~Page() = default;
    virtual void show();
    virtual void hide();
    bool is_active();
    
protected:
    lv_obj_t *page;
};

#endif // PAGE_H
