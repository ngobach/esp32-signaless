#pragma once

#include <lvgl.h>

namespace UI {
    extern lv_indev_t *ir_indev;
    
    void load_test_screen();
    void load_screen_loader();
    void load_screen_menu();
}