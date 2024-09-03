#ifndef _SETTINGS_SERVICE_H_
#define _SETTINGS_SERVICE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "message.h"

#define DEFAULT_NAME "n00b"

#define STORE_MESSAGE_COUNT 254
#define STORE_META_SIZE     sizeof(uint16_t)
#define STORE_TOTAL_SIZE    sizeof(message_pkt_t) + STORE_META_SIZE

typedef struct messages_blob {
    uint16_t count;
    message_pkt_t pkts[STORE_MESSAGE_COUNT];
} messages_blob_t;

class PersistenceService {
public:
    static PersistenceService* getInstance();

    void setName(const char *name);
    const char *getName();
    uint32_t getStateBits(const char *key, uint32_t default_val);
    bool setStateBits(const char *key, uint32_t value);
    uint8_t getAndIncrement(const char *key);
    void persistMessage(message_pkt_t *pkt);
    messages_blob_t *getMessages();

private:
    PersistenceService();
    ~PersistenceService();
    static SemaphoreHandle_t xMutex;
    static PersistenceService *instance;
    
};

#endif // _SETTINGS_SERVICE_H_