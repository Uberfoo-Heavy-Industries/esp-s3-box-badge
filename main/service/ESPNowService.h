#ifndef _ESP_NOW_SERVICE_H_
#define _ESP_NOW_SERVICE_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"

#include "espnow.h"
#include "espnow_storage.h"
#include "espnow_utils.h"

#define MAC_LEN     6
#define TTL_LEN     1
#define MSG_NUM_LEN 1
#define NAME_LEN    16
#define TEXT_LEN ESPNOW_DATA_LEN - (MAC_LEN + TTL_LEN + MSG_NUM_LEN + NAME_LEN)

#define TTL 4

typedef struct message_pkt {
    uint8_t ttl;
    uint8_t mac[MAC_LEN];
    uint8_t msg_num;
    char username[NAME_LEN];
    char text[TEXT_LEN];
} message_pkt_t;

typedef std::function<void(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl)> ESPNowSvcCb;

class ESPNowService {

private:
    wifi_init_config_t cfg;

    static ESPNowService *instance;
    static uint8_t counter;

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