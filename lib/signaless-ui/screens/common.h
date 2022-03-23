#pragma once

#include <lvgl.h>
namespace UI
{
    extern lv_indev_t *ir_indev;

    void load_test_screen();
    void load_screen_loader();
    void load_screen_menu();
    void load_screen_info();

    void msgbox_show(const char *title, const char *msg)
    {
        static lv_obj_t *msgbox;
        static lv_group_t *current_group;

        current_group = ir_indev->group;
        msgbox = lv_msgbox_create(NULL, title, msg, NULL, false);
        lv_indev_set_group(ir_indev, NULL);

        auto timer_cb = [](lv_timer_t *_)
        {
            lv_indev_set_group(ir_indev, current_group);
            lv_msgbox_close(msgbox);
        };

        lv_timer_t *timer = lv_timer_create(timer_cb, 2000, NULL);
        lv_timer_set_repeat_count(timer, 1);
    }
}