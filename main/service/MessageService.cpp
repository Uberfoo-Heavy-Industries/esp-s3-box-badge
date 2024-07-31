#include "MessageService.h"

MessageService *MessageService::instance = nullptr;
ESPNowService *MessageService::espNowService = nullptr;
SemaphoreHandle_t MessageService::xMutex = nullptr; 

MessageService *MessageService::getInstance() {
    if (instance == nullptr) {
        instance = new MessageService();
    }
    return instance;
}

MessageService::MessageService() {
    ESP_LOGI("MessageService", "starting message service");

    if (xMutex == nullptr) {
        xMutex = xSemaphoreCreateMutex();
        if (xMutex == NULL) {
            // Handle error: Mutex creation failed
            ESP_LOGE("MessageService", "Mutex creation failed!\n");
            return;
        }
    }

    espNowService = ESPNowService::getInstance();
    espNowService->registerCallback(std::bind(&MessageService::recieveMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    esp_wifi_get_mac(WIFI_IF_STA, mac);
}

void MessageService::recieveMessage(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl) {
        // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {

        ESP_LOGI("MessageService", "message recieved: %i@" MACSTR " TTL=%i", pkt->msg_num, MAC2STR(pkt->mac), pkt->ttl);

        // Check if this is our own repeated message
        if (rx_ctrl != nullptr && std::memcmp(mac, pkt->mac, 6) == 0) {
            ESP_LOGI("MessageService", "Ignoring our own message: %i@" MACSTR "", pkt->msg_num, MAC2STR(pkt->mac));
            xSemaphoreGive(xMutex);
            return;
        }
        
        // Check if we already have the message
        auto it = std::find_if(current_msgs.begin(), current_msgs.end(), [pkt](message_pkt_t *value) {
            ESP_LOGD("MessageService", "Comparing to %i " MACSTR "", value->msg_num, MAC2STR(value->mac));
            if (std::memcmp(value->mac, pkt->mac, 6) == 0 && value->msg_num == pkt->msg_num) {
                return true;
            }
            return false;
        });

        if (rx_ctrl != nullptr && it != current_msgs.end()) {
            // We already have this message ignore it
            ESP_LOGI("MessageService", "Dropping duplicate message: %i@" MACSTR "", pkt->msg_num, MAC2STR(pkt->mac));
            xSemaphoreGive(xMutex);
            return;
        }

        // Retransmit the message with decremented TTL
        message_pkt_t *pktCopy = (message_pkt_t *)heap_caps_malloc(sizeof(message_pkt_t), MALLOC_CAP_SPIRAM);
        memcpy(pktCopy, pkt, sizeof(message_pkt_t));

        if (rx_ctrl != nullptr && pkt->ttl > 0) {
            ESP_LOGI("MessageService", "retransmitting packet: ttl=%i", pkt->ttl);
            pktCopy->ttl -= 1;
            espNowService->sendPkt(pktCopy);
        }

        current_msgs.push_back(pktCopy);

        if (current_msgs.size() > 5) {
            message_pkt_t *old = current_msgs.front();
            current_msgs.pop_front();
            heap_caps_free(old);
        }

        char *username = (char *)heap_caps_malloc(NAME_LEN + 1, MALLOC_CAP_SPIRAM);
        memset(username, 0, NAME_LEN + 1);
        char *text = (char *)heap_caps_malloc(TEXT_LEN + 1, MALLOC_CAP_SPIRAM);
        memset(text, 0, TEXT_LEN + 1);
        strcpy(username, pkt->username);
        strcpy(text, pkt->text);

        ESP_LOGI("MessageService", "msg: %s: %s", username, text);
        invokeCallbacks(username, text);

        heap_caps_free(username);
        heap_caps_free(text);
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("MessageService", "Couldn't take semaphore");
    }
}

void MessageService::registerCallback(MessageSvcRcvCb callback) {
    ESP_LOGD("MessageService", "registering callback");
    callbacks.push_back(callback);
}

void MessageService::invokeCallbacks(const char *username, const char *text) {
    ESP_LOGD("MessageService", "invoking callbacks: %i", callbacks.size());
    for (const MessageSvcRcvCb &callback : callbacks) {
        ESP_LOGD("MessageService", "invoking a callback");
        callback(username, text);
    }
}