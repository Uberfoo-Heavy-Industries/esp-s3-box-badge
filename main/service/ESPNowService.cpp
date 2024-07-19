#include "ESPNowService.h"

static const char *TAG = "ESPNowService";

ESPNowService::ESPNowService() {
    espnow_storage_init();
    esp_event_loop_create_default();

    cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_init(&espnow_config);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, messageCallback);
}

ESPNowService::~ESPNowService() {

}


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

    ESP_LOGI(TAG, "espnow_recv, <%" PRIu32 "> [" MACSTR "][%d][%d][%u]: %.*s",
             count++, MAC2STR(src_addr), rx_ctrl->channel, rx_ctrl->rssi, size, size, (char *)data);

    return ESP_OK;
}

esp_err_t ESPNowService::sendMessage(const char *message) {
    xTaskCreate(
        sendTask,    // Function that should be called
        "send message",  // Name of the task (for debugging)
        1000,            // Stack size (bytes)
        (void *)message, // Parameter to pass
        1,               // Task priority
        NULL             // Task handle
    );

    return ESP_OK;
}

void ESPNowService::sendTask(void *param) {
    const char *message = (char *)param;
    static espnow_frame_head_t frame_head = {
        .broadcast        = true,
        .retransmit_count = 5,
    };

    ESP_LOGI(TAG, "sending text: %s", message);
    esp_err_t ret = espnow_send(ESPNOW_DATA_TYPE_DATA, ESPNOW_ADDR_BROADCAST, message, strlen(message), &frame_head, portMAX_DELAY);
    ESP_LOGI(TAG, "sent text: %s", message);

    vTaskDelete(NULL);
}