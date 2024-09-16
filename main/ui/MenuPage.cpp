#include "MenuPage.h"
#include "NamePage.h"
#include "MessagePage.h"
#include "DemoSettingsPage.h"
#include "MessageLogPage.h"
#include "MainPage.h"

// Initialize the static member
MenuPage* MenuPage::instance = nullptr;

MenuPage::MenuPage(lv_obj_t *parent) : Page(parent) {
    
    // Create a container to hold the grid
    lv_obj_t *cont = lv_obj_create(page);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100)); // Container takes full parent size
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN);

    // Define the grid layout (2 rows, 2 columns)
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};  // 2 columns
    static lv_coord_t row_dsc[] = {LV_GRID_FR(2), LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};  // 2 rows
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    // Create 4 buttons and place them in the grid
    for (int i = 0; i < 4; i++) {
        lv_obj_t *btn = lv_btn_create(cont); // Create button
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i % 2, 1, LV_GRID_ALIGN_STRETCH, i / 2, 1); // Grid position

        // Create a label on the button
        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text_fmt(label, labels[i], i + 1); // Button 1 to 4

        // Center the label inside the button
        lv_obj_center(label);
 
        uint8_t* btn_option = new uint8_t(i);
        lv_obj_add_event_cb(btn, options_btn_event_cb, LV_EVENT_CLICKED, btn_option);
    }

    // Set the container to use grid layout
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    // Create a back button to return to the main page
    lv_obj_t *back_btn = lv_btn_create(cont);
    lv_obj_t *back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");
    lv_obj_center(back_btn_label);
    lv_obj_set_grid_cell(back_btn, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 2, 1); // Grid position
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, this);
}

MenuPage* MenuPage::getInstance(lv_obj_t *parent) {
    if (instance == nullptr && parent != nullptr) {
        instance = new MenuPage(parent);
    }
    return instance;
}

void MenuPage::show() {
    Page::show();
}

void MenuPage::options_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        uint8_t option = *static_cast<uint8_t*>(lv_event_get_user_data(e));
        MenuPage::getInstance()->hide();
        switch (option) {
        case 0:
            NamePage::getInstance()->show();
            break;

        case 1:
            MessagePage::getInstance()->show();
            break;

        case 2:
            MessageLogPage::getInstance()->show();
            break;

        case 3:
            DemoSettingsPage::getInstance()->show();
        }
    }
}

void MenuPage::back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        MenuPage::getInstance()->hide();
        MainPage::getInstance()->show();
    }
}

