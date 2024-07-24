#include "MessageService.h"

MessageService *MessageService::instance = nullptr;
ESPNowService *MessageService::espNowService = nullptr;

MessageService *MessageService::getInstance() {
    if (instance == nullptr) {
        instance = new MessageService();
    }
    return instance;
}

MessageService::MessageService() {
    ESP_LOGI("MessageService", "starting message service");
    espNowService = ESPNowService::getInstance();
    espNowService->registerCallback(std::bind(&MessageService::recieveMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void MessageService::recieveMessage(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl) {
    ESP_LOGI("MessageService", "message recieved: %i@" MACSTR " TTL=%i", pkt->msg_num, MAC2STR(pkt->mac), pkt->ttl);
    
    // Check if we already have the message
    auto it = std::find_if(current_msgs.begin(), current_msgs.end(), [pkt](message_pkt_t *value) {
        ESP_LOGD("MessageService", "Comparing to %i " MACSTR "", value->msg_num, MAC2STR(value->mac));
        if (std::memcmp(value->mac, pkt->mac, 6) == 0 && value->msg_num == pkt->msg_num) {
            return true;
        }
        return false;
    });

    if (it == current_msgs.end()) {
        // We already have this message ignore it
        ESP_LOGI("MessageService", "Dropping duplicate message: %i@" MACSTR "", pkt->msg_num, MAC2STR(pkt->mac));
        return;
    }

    // Retransmit the message with decremented TTL
    message_pkt_t *pktCopy = (message_pkt_t *)heap_caps_malloc(sizeof(message_pkt_t), MALLOC_CAP_SPIRAM);
    memcpy(pktCopy, pkt, sizeof(message_pkt_t));

    if (pkt->ttl > 0) {
        ESP_LOGI("MessageService", "retransmitting packet: ttl=%i", pkt->ttl);
        pktCopy->ttl -= 1;
        espNowService->sendPkt(pktCopy);
    }

    current_msgs.push_back(pktCopy);
}