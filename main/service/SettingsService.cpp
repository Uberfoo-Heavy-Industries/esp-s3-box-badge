#include "SettingsService.h"

SettingsService *SettingsService::instance = nullptr;
SemaphoreHandle_t SettingsService::xMutex = nullptr; 

SettingsService::SettingsService() {
    if (xMutex == nullptr) {
        xMutex = xSemaphoreCreateMutex();
        if (xMutex == NULL) {
            // Handle error: Mutex creation failed
            ESP_LOGE("SettingsService", "Mutex creation failed!\n");
            return;
        }
    }

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {

            // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("SettingsService", "Couldn't take semaphore");
    }

}

SettingsService::~SettingsService() {
    if (xMutex != NULL) {
        vSemaphoreDelete(xMutex);
    }
}

SettingsService *SettingsService::getInstance() {
    if (instance == nullptr) {
        instance = new SettingsService();
    }
    return instance;
}

void SettingsService::setName(const char *name) {

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        esp_err_t ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE("SettingsService", "Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        } else {
            // Write value to NVS
            ret = nvs_set_str(handle, "name", name);
            if (ret != ESP_OK) {
                ESP_LOGE("SettingsService", "Failed to write name! name: %s", name);
            }
            ESP_LOGD("SettingsService", "Write name successful! name: %s", name);

            // Commit written value
            ret = nvs_commit(handle);
            if (ret != ESP_OK) {
                ESP_LOGE("SettingsService", "Failed to commit name! name: %s", name);
            }
            ESP_LOGD("SettingsService", "Commit name successful! name: %s", name);

            // Close NVS handle
            nvs_close(handle);
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("SettingsService", "Couldn't take semaphore");
    }
}

const char *SettingsService::getName() {
        // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // Read value from NVS
        esp_err_t ret = nvs_open("storage", NVS_READONLY, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE("SettingsService", "Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
            setName(DEFAULT_NAME);
        } else {
            size_t size = 0;
            char *value_read;
            ret = nvs_get_str(handle, "name", 0, &size);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD("SettingsService", "Value size: %d", size);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    ESP_LOGE("SettingsService", "The name value is not initialized yet!");
                    nvs_close(handle);
                    xSemaphoreGive(xMutex);
                    return DEFAULT_NAME; // Just return the default
                default :
                    ESP_LOGE("SettingsService", "Error (%s) reading !\n", esp_err_to_name(ret));
            }


            value_read = (char *)heap_caps_malloc(size, MALLOC_CAP_INTERNAL);
            ret = nvs_get_str(handle, "name", value_read, &size);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD("SettingsService", "Value read: %s", value_read);
                    break;
                default :
                    ESP_LOGE("SettingsService", "Error (%s) reading !", esp_err_to_name(ret));
            }

            // Close NVS handle
            nvs_close(handle);
            xSemaphoreGive(xMutex);
            return value_read;
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("SettingsService", "Couldn't take semaphore");
    }

    return nullptr;
}

uint8_t SettingsService::getAndIncrement(const char *key) {

    uint8_t value = 0;

    // Attempt to take the mutex, wait indefinitely if necessary
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
        esp_err_t ret = nvs_open("storage", NVS_READWRITE, &handle);
        if (ret != ESP_OK) {
            ESP_LOGE("SettingsService", "Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
            setName(DEFAULT_NAME);
        } else {
            ret = nvs_get_u8(handle, key, &value);
            switch (ret) {
                case ESP_OK:
                    ESP_LOGD("SettingsService", "%s value: %d", key, value);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    ESP_LOGI("SettingsService", "The %s value is not initialized yet. Setting to default.", key);
                    ret = nvs_set_u8(handle, key, 1); // set the incremented default
                    if (ret != ESP_OK) {
                        ESP_LOGE("SettingsService", "Error (%s) writing default %s!\n", esp_err_to_name(ret), key);
                    }
                    nvs_commit(handle);
                    nvs_close(handle);
                    xSemaphoreGive(xMutex);
                    return 0; // Just return the default
                default :
                    ESP_LOGE("SettingsService", "Error (%s) reading %s!\n", esp_err_to_name(ret), key);
            }
            ret = nvs_set_u8(handle, key, value + 1);
            if (ret != ESP_OK) {
                ESP_LOGE("SettingsService", "Error (%s) incrementing and writing %s!\n", esp_err_to_name(ret), key);
            }
            nvs_commit(handle);
            nvs_close(handle);
            xSemaphoreGive(xMutex);
            return value;
        }
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("SettingsService", "Couldn't take semaphore");
    }
    return value;

}