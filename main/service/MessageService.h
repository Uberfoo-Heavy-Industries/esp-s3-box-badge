#ifndef _MESSAGE_SERVICE_H_
#define _MESSAGE_SERVICE_H_

#include <iostream>
#include <list>
#include <algorithm>
#include <stdint.h>
#include <cstring>

#include "bsp/esp-bsp.h"
#include "esp_now.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ESPNowService.h"
#include "PersistenceService.h"
#include "AudioService.h"
#include "message.h"

typedef std::function<void(const char *username, const char *text)> MessageSvcRcvCb;

class MessageService {

private:
    static MessageService *instance;
    static ESPNowService *espNowService;
    static PersistenceService *persistenceService;
    static AudioService *audioService;

    std::vector<MessageSvcRcvCb> callbacks;

    void invokeCallbacks(const char *username, const char *text);

public:
    std::list<message_pkt_t *> current_msgs;
    uint8_t mac[6];
    
    MessageService();

    void recieveMessage(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl);
    void registerCallback(MessageSvcRcvCb callback);

    static SemaphoreHandle_t xMutex;
    static MessageService *getInstance();
};

#endif // _MESSAGE_SERVICE_H_