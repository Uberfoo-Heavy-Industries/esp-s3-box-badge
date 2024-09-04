#ifndef _MAINPAGE_H_
#define _MAINPAGE_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "Page.h"
#include "demo.h"

#define DEFAULT_DEMO_STATE  (DEMO_FIRE | DEMO_METABALLS | DEMO_DEFORM | DEMO_ROTOZOOM | DEMO_PLASMA)
#define DEMO_TIMEOUT        pdMS_TO_TICKS(10000)

typedef struct demo_task_params {
    lv_obj_t *canvas;
    uint16_t width;
    uint16_t height;
} demo_task_params;


class MainPage : public Page {
public:
    static MainPage* getInstance(lv_obj_t *parent = nullptr);
    static void demo_task(void *obj);
    static void load_task(void *obj);
    static void demo_timer_cb(TimerHandle_t handle);
    void show() override;
    void hide() override;

    Demo *currentDemo = nullptr;
    char *demoBuf = nullptr;

private:
    MainPage(lv_obj_t *parent);
    static void menu_btn_event_cb(lv_event_t *e);

    void getNextIndex();
    void loadDemo();
    
    int index = -1;
    bool switch_flag = false;
    bool run = true;
    uint32_t state;
    demo_task_params params;
    TaskHandle_t task_handle = nullptr;
    TaskHandle_t task_handle2 = nullptr;
    TimerHandle_t timer_handle = nullptr;
    lv_color_t *buf;
    lv_obj_t *canvas;
    lv_obj_t *top_pane;
    lv_obj_t *label;
    lv_obj_t *new_msg_img;
    SemaphoreHandle_t lock;
    static MainPage *instance;
};

#endif // _MAINPAGE_H_

