#include "ESPNowService.h"
#include "PersistenceService.h"

static const char *TAG = "ESPNowService";

ESPNowService *ESPNowService::instance = nullptr;
PersistenceService *ESPNowService::persistenceService = nullptr;

ESPNowService::ESPNowService() {
    persistenceService = PersistenceService::getInstance();

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

    messageQueue = xQueueCreate(5, sizeof(message_pkt_t)); // Queue can hold 10 Message structures
    if (messageQueue == NULL) {
        // Handle error: Queue creation failed
        printf("Failed to create queue.\n");
        return;
    }

    xTaskCreate(sendTask, "send_message", 4096, &messageQueue, 0, NULL);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, messageCallback);
}

ESPNowService::~ESPNowService() {

}

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
    pkt->msg_num = persistenceService->getAndIncrement("msg_count");
    pkt->ttl = TTL;
    const char *name = PersistenceService::getInstance()->getName();
    strncpy(pkt->username, name, NAME_LEN);
    strncpy(pkt->text, message, TEXT_LEN);

    esp_err_t ret = sendPkt(pkt);
    if (ret != ESP_OK) {
        return ret;
    }
    invokeCallbacks(pkt->mac, pkt, nullptr);
    return ESP_OK;
}

esp_err_t ESPNowService::sendPkt(const message_pkt_t *pkt) {
    ESP_LOGI(TAG, "sending pkt: %i", pkt->msg_num);

    // Send the message to the queue
    if (xQueueSend(messageQueue, pkt, portMAX_DELAY) != pdPASS) {
        // Handle error: Failed to send message
        ESP_LOGE(TAG, "Failed to send pkt to queue: %i", pkt->msg_num);
        return -1;
    }

    return ESP_OK;
}

void ESPNowService::sendTask(void *param) {
    QueueHandle_t *messageQueue = (QueueHandle_t *)param;
    while (1) {
        message_pkt_t pkt;
        if (xQueueReceive(*messageQueue, &pkt, portMAX_DELAY) == pdPASS) {
            static espnow_frame_head_t frame_head = {
                .broadcast        = true,
                .retransmit_count = 5,
            };

            ESP_ERROR_CHECK(espnow_send(ESPNOW_DATA_TYPE_DATA, ESPNOW_ADDR_BROADCAST, &pkt, sizeof(message_pkt_t), &frame_head, portMAX_DELAY));
            
            ESP_LOGI(TAG, "sent message %i: %s", pkt.msg_num, pkt.text);
        }
    }
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