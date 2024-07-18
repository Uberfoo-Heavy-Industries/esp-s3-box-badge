#ifndef _ESP_NOW_SERVICE_H_
#define _ESP_NOW_SERVICE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"

#include "espnow.h"
#include "espnow_storage.h"
#include "espnow_utils.h"

class ESPNowService {

private:
    wifi_init_config_t cfg;
    static ESPNowService *instance;

public:
    ESPNowService();
    ~ESPNowService();

    esp_err_t sendMessage(const char *message);

    static esp_err_t messageCallback(uint8_t *src_addr, void *data,
        size_t size, wifi_pkt_rx_ctrl_t *rx_ctrl);
    
    static ESPNowService *getInstance();
};

#endif // _ESP_NOW_SERVICE_H_