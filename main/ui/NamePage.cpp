#include <stdio.h>
#include "NamePage.h"
#include "PersistenceService.h"

NamePage* NamePage::instance = nullptr;

NamePage::NamePage(lv_obj_t *parent) : TextEntryPage(parent, "Save") {}

void NamePage::performAction() {
    // Handle save action
    const char *text = lv_textarea_get_text(text_area);
    // Save the text or perform any other action
    ESP_LOGD("NamePage", "Saving text: %s", text);

    PersistenceService::getInstance()->setName(text);
}

NamePage* NamePage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new NamePage(parent);
    }
    return instance;
}
