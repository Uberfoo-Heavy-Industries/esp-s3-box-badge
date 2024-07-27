#ifndef PAGE_H
#define PAGE_H

#include <vector>

#include "lvgl.h"

class Page {
public:
    Page(lv_obj_t *parent);
    virtual ~Page() = default;
    virtual void show();
    virtual void hide();
    bool is_active();

    static std::vector<Page*> pages;
    static void hideAll();
    
protected:
    lv_obj_t *page;
};

#endif // PAGE_H
