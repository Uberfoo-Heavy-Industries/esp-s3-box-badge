#include <stdio.h>
#include "NamePage.h"
#include "PersistenceService.h"

NamePage* NamePage::instance = nullptr;

NamePage::NamePage(lv_obj_t *parent) : TextEntryPage(parent, "Save") {}

void NamePage::show() {
    bsp_display_lock(0);
    
    const char *name = PersistenceService::getInstance()->getName();
    lv_textarea_set_text(text_area, name);

    Page::show();

    bsp_display_unlock();
}

void NamePage::performAction() {
    // Handle save action
    const char *text = lv_textarea_get_text(text_area);
    // Save the text or perform any other action
    ESP_LOGD("NamePage::performAction()", "Saving text: %s", text);

    PersistenceService::getInstance()->setName(text);
}

NamePage* NamePage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new NamePage(parent);
    }
    return instance;
}
