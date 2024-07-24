#ifndef _MAINPAGE_H_
#define _MAINPAGE_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Page.h"

typedef struct demo_task_params {
    lv_obj_t *canvas;
    uint16_t width;
    uint16_t height;
} demo_task_params;


class MainPage : public Page {
public:
    static MainPage* getInstance(lv_obj_t *parent = nullptr);
    static void demo_task(void *obj);
    void show() override;
    void hide() override;

private:
    MainPage(lv_obj_t *parent);
    static void menu_btn_event_cb(lv_event_t *e);

    demo_task_params params;
    TaskHandle_t task_handle = nullptr;
    lv_color_t *buf;
    lv_obj_t *canvas;
    lv_obj_t *top_pane;
    lv_obj_t *label;
    lv_obj_t *new_msg_img;
    static MainPage *instance;
};

#endif // _MAINPAGE_H_

