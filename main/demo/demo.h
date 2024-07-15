#ifndef _DEMO_H_
#define _DEMO_H_

#include "lvgl.h"

class Demo {
    protected:
    lv_obj_t *canvas;
    uint16_t width;
    uint16_t height;

    public:
    Demo(lv_obj_t *canvas, uint16_t width, uint16_t height);

    virtual void renderFrame() = 0;
};

#endif // _DEMO_H_