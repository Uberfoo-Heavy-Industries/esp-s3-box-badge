#include "PersistenceService.h"

static const char *TAG = "PersistenceService";

PersistenceService *PersistenceService::instance = nullptr;
SemaphoreHandle_t PersistenceService::xMutex = nullptr; 

PersistenceService::PersistenceService() {
    if (xMutex == nullptr) {
        xMutex = xSemaphoreCreateMutex();
        if (xMutex == NULL) {
            // Handle error: Mutex creation failed
            ESP_LOGE(TAG, "Mutex creation failed!\n");
            return;
        }
    }

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {

            // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_LOGI(TAG, "nvs init failed, erasing nvs");
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
        ESP_LOGI(TAG, "nvs initialized");
    
        nvs_handle_t handle;
        ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) opening NVS handle! (140)\n", esp_err_to_name(ret));
        } else {
            nvs_close(handle);
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Couldn't take semaphore");
    }

}

PersistenceService::~PersistenceService() {
    if (xMutex != NULL) {
        vSemaphoreDelete(xMutex);
    }
}

PersistenceService *PersistenceService::getInstance() {
    if (instance == nullptr) {
        instance = new PersistenceService();
    }
    return instance;
}

void PersistenceService::setName(const char *name) {

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        nvs_handle_t handle;
        esp_err_t ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle! (57)\n", esp_err_to_name(ret));
        } else {
            // Write value to NVS
            ret = nvs_set_str(handle, "name", name);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to write name! name: %s", name);
            }
            ESP_LOGD(TAG, "Write name successful! name: %s", name);

            // Commit written value
            ret = nvs_commit(handle);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to commit name! name: %s", name);
            }
            ESP_LOGD(TAG, "Commit name successful! name: %s", name);

            // Close NVS handle
            nvs_close(handle);
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Couldn't take semaphore");
    }
}

const char *PersistenceService::getName() {
        // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        // Read value from NVS
        nvs_handle_t handle;
        esp_err_t ret = nvs_open("storage", NVS_READONLY, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle! (89)\n", esp_err_to_name(ret));
        } else {
            size_t size = 0;
            char *value_read;
            ret = nvs_get_str(handle, "name", 0, &size);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD(TAG, "Value size: %d", size);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    ESP_LOGE(TAG, "The name value is not initialized yet!");
                    nvs_close(handle);
                    xSemaphoreGive(xMutex);
                    return DEFAULT_NAME; // Just return the default
                default :
                    ESP_LOGE(TAG, "Error (%s) reading !\n", esp_err_to_name(ret));
            }


            value_read = (char *)heap_caps_malloc(size, MALLOC_CAP_INTERNAL);
            ret = nvs_get_str(handle, "name", value_read, &size);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD(TAG, "Value read: %s", value_read);
                    break;
                default :
                    ESP_LOGE(TAG, "Error (%s) reading !", esp_err_to_name(ret));
            }

            // Close NVS handle
            nvs_close(handle);
            xSemaphoreGive(xMutex);
            return value_read;
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Couldn't take semaphore");
    }

    return nullptr;
}

uint8_t PersistenceService::getAndIncrement(const char *key) {

    uint8_t value = 0;

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        nvs_handle_t handle;
        esp_err_t ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle! (140)\n", esp_err_to_name(ret));
        } else {
            ret = nvs_get_u8(handle, key, &value);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD(TAG, "%s value: %d", key, value);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    ESP_LOGI(TAG, "The %s value is not initialized yet. Setting to default.", key);
                    ret = nvs_set_u8(handle, key, 1); // set the incremented default
                    if (ret != ESP_OK) {
                        ESP_LOGE(TAG, "Error (%s) writing default %s!\n", esp_err_to_name(ret), key);
                    }
                    nvs_commit(handle);
                    nvs_close(handle);
                    xSemaphoreGive(xMutex);
                    return 0; // Just return the default
                default :
                    ESP_LOGE(TAG, "Error (%s) reading %s!\n", esp_err_to_name(ret), key);
            }
            ret = nvs_set_u8(handle, key, value + 1);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) incrementing and writing %s!\n", esp_err_to_name(ret), key);
            }
            nvs_commit(handle);
            nvs_close(handle);
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Couldn't take semaphore");
    }
    return value;

}

void PersistenceService::persistMessage(message_pkt_t *pkt) {

    messages_blob_t *blob = getMessages();
    if (blob->count < STORE_MESSAGE_COUNT) {
        blob->pkts[blob->count++] = *pkt;
    } else {
        for (uint16_t i = 0; i < STORE_MESSAGE_COUNT - 1; i++) {
            blob->pkts[i] = blob->pkts[i+1];
        }
        blob->pkts[STORE_MESSAGE_COUNT - 1] = *pkt;
    }

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        nvs_handle_t handle;
        esp_err_t ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle! (192)\n", esp_err_to_name(ret));
        } else {
            ret = nvs_set_blob(handle, "messages", blob, sizeof(messages_blob_t)); // set the incremented default
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing new messages blob!", esp_err_to_name(ret));
            } else {
                ESP_LOGI(TAG, "messages persisted");
            }
            nvs_commit(handle);
            nvs_close(handle);
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Couldn't take semaphore");
    }

    heap_caps_free(blob);
}

messages_blob_t *PersistenceService::getMessages() {
    size_t size = sizeof(messages_blob_t);
    messages_blob_t *value = (messages_blob_t *)heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    memset(value, 0, size);
    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        nvs_handle_t handle;
        esp_err_t ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle! (220)\n", esp_err_to_name(ret));
        } else {
            ret = nvs_get_blob(handle, "messages", value, &size);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD(TAG, "retrieved %d messages", value->count);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    ESP_LOGI(TAG, "The message value is not initialized yet. Setting to empty.");
                    ret = nvs_set_blob(handle, "messages", value, size); // set the incremented default
                    if (ret != ESP_OK) {
                        ESP_LOGE(TAG, "Error (%s) writing default messages blob!", esp_err_to_name(ret));
                    }
                    nvs_commit(handle);
                    break;
                default:
                    ESP_LOGE(TAG, "Error (%s) reading messages blob!", esp_err_to_name(ret));
            }
            nvs_close(handle);
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE(TAG, "Couldn't take semaphore");
    }
    return value;
}