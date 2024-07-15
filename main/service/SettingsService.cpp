#include "SettingsService.h"

SettingsService *SettingsService::instance = nullptr;

SettingsService::SettingsService() {
        // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

}

SettingsService *SettingsService::getInstance() {
    if (instance == nullptr) {
        instance = new SettingsService();
    }
    return instance;
}

void SettingsService::setName(const char *name) {
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

}

char *SettingsService::getName() {
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

        return value_read;
    }

    return nullptr;
}