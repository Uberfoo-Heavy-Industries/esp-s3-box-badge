#ifndef _SETTINGS_SERVICE_H_
#define _SETTINGS_SERVICE_H_

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#define DEFAULT_NAME "n00b"

class SettingsService {
public:
    static SettingsService* getInstance();

    void setName(const char *name);
    char *getName();


private:
    SettingsService();
    nvs_handle_t handle;

    static SettingsService *instance;
};

#endif // _SETTINGS_SERVICE_H_