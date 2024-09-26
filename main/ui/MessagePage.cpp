#include "MessagePage.h"
#include "ESPNowService.h"

MessagePage* MessagePage::instance = nullptr;

MessagePage::MessagePage(lv_obj_t *parent) : TextEntryPage(parent, "Send") {}

MessagePage *MessagePage::getInstance(lv_obj_t *parent)
{
    if (instance == nullptr && parent != nullptr) {
        instance = new MessagePage(parent);
    }
    return instance;
}

void MessagePage::performAction()
{
        // Handle save 
        const char *text = lv_textarea_get_text(text_area);        // Save the text or perform any other action
        ESP_LOGI("MessagePage", "Sending text: %s", text);

        ESPNowService::getInstance()->sendMessage(text);

        lv_textarea_set_text(text_area, "");

}