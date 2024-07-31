#include "AudioService.h"

static const char *TAG = "AudioService";

static esp_err_t audio_mute_function(AUDIO_PLAYER_MUTE_SETTING setting)
{
    // Volume saved when muting and restored when unmuting. Restoring volume is necessary
    // as es8311_set_voice_mute(true) results in voice volume (REG32) being set to zero.
    uint8_t volume = 75;

    bsp_codec_mute_set(setting == AUDIO_PLAYER_MUTE ? true : false);

    // restore the voice volume upon unmuting
    if (setting == AUDIO_PLAYER_UNMUTE) {
        bsp_codec_volume_set(volume, NULL);
    }

    return ESP_OK;
}

AudioService *AudioService::instance = nullptr;
TaskHandle_t *AudioService::task = nullptr;
QueueHandle_t AudioService::queue;

AudioService::AudioService() {
    queue = xQueueCreate(5, sizeof(uint8_t));
    xTaskCreate(AudioService::playAudio, "play_audio", 4096, &queue, 0, task);
}

AudioService *AudioService::getInstance() {
    if (instance == nullptr) {
        instance = new AudioService();
    }
    return instance;
}

void AudioService::playNotification() {
    uint8_t index = 0;
    xQueueSend(queue, &index, portMAX_DELAY);
}

void AudioService::playAudio(void *param) {
    QueueHandle_t *queue = (QueueHandle_t *)param;

    audio_player_config_t config = { .mute_fn = audio_mute_function,
                                     .clk_set_fn = bsp_codec_set_fs,
                                     .write_fn = bsp_i2s_write,
                                     .priority = 1
                                   };
    ESP_ERROR_CHECK(audio_player_new(config));

    while (1) {
        uint8_t num;
        if (xQueueReceive(*queue, &num, portMAX_DELAY) == pdPASS) {

            FILE *fp = fopen(BSP_SPIFFS_MOUNT_POINT"/uh-ooh_01.mp3", "rb");
            if (fp) {
                ESP_LOGI(TAG, "Playing '%s'", "/uh-ooh_01.mp3");
                audio_player_play(fp);
            } else {
                ESP_LOGE(TAG, "unable to open filename '%s'", "/uh-ooh_01.mp3");
            }

        }
    }

}