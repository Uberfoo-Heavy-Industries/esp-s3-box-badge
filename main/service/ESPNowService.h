#ifndef _ESP_NOW_SERVICE_H_
#define _ESP_NOW_SERVICE_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"

#include "espnow.h"
#include "espnow_storage.h"
#include "espnow_utils.h"

#include "PersistenceService.h"
#include "message.h"

#define TTL 4

typedef std::function<void(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl)> ESPNowSvcCb;

class ESPNowService {

private:
    wifi_init_config_t cfg;
    QueueHandle_t messageQueue;

    static ESPNowService *instance;
    static PersistenceService *settingsService;

    static esp_err_t messageCallback(uint8_t *src_addr, void *data, size_t size, wifi_pkt_rx_ctrl_t *rx_ctrl);
    static void sendTask(void *param);

    void invokeCallbacks(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl);
    

public:
    
    ESPNowService();
    ~ESPNowService();

    esp_err_t sendMessage(const char *message);
    esp_err_t sendPkt(const message_pkt_t *pkt);

    void registerCallback(ESPNowSvcCb callback);

    static ESPNowService *getInstance();

private:
    std::vector<ESPNowSvcCb> callbacks;

};

#endif // _ESP_NOW_SERVICE_H_