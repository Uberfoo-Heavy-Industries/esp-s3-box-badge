#include "lvgl.h"

class Demo {
    protected:
    lv_obj_t *canvas;

    public:
    Demo(lv_obj_t *canvas);

    virtual void renderFrame() = 0;
};
