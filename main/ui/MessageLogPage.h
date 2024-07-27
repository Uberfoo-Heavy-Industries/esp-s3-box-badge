#ifndef _MESSAGE_LOG_PAGE_H
#define _MESSAGE_LOG_PAGE_H

#include <lvgl.h>
#include <vector>
#include <string>
#include "Page.h"
#include "MessageService.h"

class MessageLogPage : public Page {

public:
    static MessageLogPage* getInstance(lv_obj_t *parent = nullptr);
    
    void show() override;
    void addMessage(const char *username, const char *text);

private:
    MessageService *messageService;
    MessageLogPage(lv_obj_t *parent);
    lv_obj_t *list;
    std::vector<char *> messages;

    static MessageLogPage *instance;
};

#endif // _MESSAGE_LOG_PAGE_H