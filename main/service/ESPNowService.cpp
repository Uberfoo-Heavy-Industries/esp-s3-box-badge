#include "ESPNowService.h"
#include "SettingsService.h"

static const char *TAG = "ESPNowService";

ESPNowService::ESPNowService() {
    espnow_storage_init();

    cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
    
    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_init(&espnow_config);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, messageCallback);
}

ESPNowService::~ESPNowService() {

}

uint8_t ESPNowService::counter = 0;
ESPNowService *ESPNowService::instance = nullptr;

ESPNowService *ESPNowService::getInstance() {
    if (instance == nullptr) {
        instance = new ESPNowService();
    }
    return instance;
}

esp_err_t ESPNowService::messageCallback(uint8_t *src_addr, void *data,
                                       size_t size, wifi_pkt_rx_ctrl_t *rx_ctrl)
{
    ESP_PARAM_CHECK(src_addr);
    ESP_PARAM_CHECK(data);
    ESP_PARAM_CHECK(size);
    ESP_PARAM_CHECK(rx_ctrl);

    static uint32_t count = 0;

    message_pkt_t *pkt = (message_pkt_t *)data;

    ESP_LOGI(TAG, "espnow_recv, <%" PRIu32 "> [" MACSTR "][%d][%d][%u]: %.*s",
             count++, MAC2STR(src_addr), rx_ctrl->channel, rx_ctrl->rssi, size, TEXT_LEN, pkt->text);

    getInstance()->invokeCallbacks(src_addr, pkt, rx_ctrl);
    return ESP_OK;
}

esp_err_t ESPNowService::sendMessage(const char *message) {
    ESP_LOGI(TAG, "sending text: %s", message);

    message_pkt_t *pkt = (message_pkt_t *)heap_caps_malloc(sizeof(message_pkt_t), MALLOC_CAP_SPIRAM);
    esp_wifi_get_mac(WIFI_IF_STA, pkt->mac);
    pkt->msg_num = counter++;
    pkt->ttl = TTL;
    const char *name = SettingsService::getInstance()->getName();
    strncpy(pkt->username, name, NAME_LEN);
    strncpy(pkt->text, message, TEXT_LEN);

    return sendPkt(pkt);
}

esp_err_t ESPNowService::sendPkt(const message_pkt_t *pkt) {
    ESP_LOGI(TAG, "sending pkt: %i", pkt->msg_num);
    message_pkt_t *clone = (message_pkt_t*)heap_caps_malloc(sizeof(message_pkt_t), MALLOC_CAP_SPIRAM);
    memcpy(clone, pkt, sizeof(message_pkt_t));
    xTaskCreate(
        sendTask,        // Function that should be called
        "send packet",  // Name of the task (for debugging)
        2048,            // Stack size (bytes)
        (void *)clone,     // Parameter to pass
        1,               // Task priority
        NULL             // Task handle
    );

    return ESP_OK;

}

void ESPNowService::sendTask(void *param) {
    const message_pkt_t *pkt = (message_pkt_t *)param;
    static espnow_frame_head_t frame_head = {
        .broadcast        = true,
        .retransmit_count = 5,
    };

    ESP_ERROR_CHECK(espnow_send(ESPNOW_DATA_TYPE_DATA, ESPNOW_ADDR_BROADCAST, pkt, sizeof(message_pkt_t), &frame_head, portMAX_DELAY));
    
    ESP_LOGI(TAG, "sent message %i: %s", pkt->msg_num, pkt->text);

    heap_caps_free((void *)pkt);
    vTaskDelete(NULL);
}

void ESPNowService::registerCallback(ESPNowSvcCb callback) {
    ESP_LOGI(TAG, "registering callback");
    callbacks.push_back(callback);
}

void ESPNowService::invokeCallbacks(uint8_t *src_addr, message_pkt_t *pkt, wifi_pkt_rx_ctrl_t *rx_ctrl) {
    ESP_LOGI(TAG, "invoking callbacks: %i", callbacks.size());
    for (const ESPNowSvcCb &callback : callbacks) {
        ESP_LOGI(TAG, "invoking a callback");
        callback(src_addr, pkt, rx_ctrl);
    }
}