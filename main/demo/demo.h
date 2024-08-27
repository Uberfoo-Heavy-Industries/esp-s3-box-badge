#ifndef _DEMO_H_
#define _DEMO_H_

#include "esp_log.h"
#include "lvgl.h"

#define DEMO_FIRE       (1)
#define DEMO_METABALLS  (1 << 1)
#define DEMO_DEFORM     (1 << 2)
#define DEMO_ROTOZOOM   (1 << 3)
#define DEMO_COUNT      4

class Demo {
    protected:
    lv_obj_t *canvas;
    uint16_t width;
    uint16_t height;

    public:
    Demo(lv_obj_t *canvas, uint16_t width, uint16_t height);
    virtual ~Demo() {
        ESP_LOGI("Demo", "demo deconstructor");
    };

    virtual void renderFrame() = 0;
};

#endif // _DEMO_H_