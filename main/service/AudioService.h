#ifndef _AUDIO_SERVICE_H_
#define _AUDIO_SERVICE_H_

#include "bsp/esp-bsp.h"
#include "esp_check.h"
#include "esp_log.h"
#include "bsp_board.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "audio_player.h"

#define SOUND_ID_NOTIFICATION 0

class AudioService {

private:
    static AudioService *instance;
    static QueueHandle_t queue;
    static TaskHandle_t *task;
    static void playAudio(void *param);

    AudioService();

public:
    static AudioService *getInstance();

    void playNotification();

};

#endif // _AUDIO_SERVICE_H_