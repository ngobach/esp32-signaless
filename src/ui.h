#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "lv_indev_ir.h"
#include "badwifi.h"

namespace UI
{
    static const uint16_t screenWidth = TFT_WIDTH;
    static const uint16_t screenHeight = TFT_HEIGHT;
    TFT_eSPI tft = TFT_eSPI();
    lv_indev_t *ir_indev;

    void tft_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
    {
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);
        tft.startWrite();
        tft.setAddrWindow(area->x1, area->y1, w, h);
        tft.pushColors((uint16_t *)&color_p->full, w * h, true);
        tft.endWrite();
        lv_disp_flush_ready(disp);
    }

    void load_screen_loader();
    void load_screen_menu();
    void load_screen_scan();

    void setup()
    {
        static lv_disp_draw_buf_t draw_buf;
        static lv_color_t buf[screenWidth * 10];
        lv_init();
        tft.begin();
        tft.setRotation(0);
        lv_disp_draw_buf_init(&draw_buf, buf, NULL, sizeof(buf) / sizeof(lv_color_t));
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = screenWidth;
        disp_drv.ver_res = screenHeight;
        disp_drv.flush_cb = tft_disp_flush;
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register(&disp_drv);
        ir_indev = lv_indev_drv_register(lv_indev_drv_ir_create());

        // Finally
        load_screen_loader();
    }

    void tick()
    {
        static uint32_t last_draw = millis();
        uint32_t ellapsed = millis() - last_draw;
        last_draw = millis();
        lv_timer_handler();
        lv_tick_inc(ellapsed);
    }

    /**
     * Screens
     */

    void load_screen_loader()
    {
        lv_obj_t *scr = lv_obj_create(NULL);
        lv_obj_t *spinner = lv_spinner_create(scr, 1000, 60);
        lv_obj_set_size(spinner, 80, 80);
        lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -20);
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, "Loading...");
        lv_obj_align_to(label, spinner, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
        lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, true);

        lv_timer_t *timer = lv_timer_create(
            [](lv_timer_t *x)
            {
                load_screen_menu();
            },
            2000,
            nullptr);
        lv_timer_set_repeat_count(timer, 1);
    }

    void load_screen_menu()
    {
        lv_group_t *group = lv_group_create();
        lv_obj_t *menu = lv_list_create(nullptr);

        lv_list_add_text(menu, "ESP32-Signaless");

        lv_obj_t *btn_targets = lv_list_add_btn(menu, LV_SYMBOL_LIST, "Target APs");
        lv_obj_add_event_cb(
            btn_targets, [](lv_event_t *e)
            { Serial.println("Pressed Targets"); },
            LV_EVENT_PRESSED, nullptr);
        lv_group_add_obj(group, btn_targets);

        lv_obj_t *btn_scan = lv_list_add_btn(menu, LV_SYMBOL_WIFI, "Scan");
        lv_obj_add_event_cb(
            btn_scan, [](lv_event_t *e)
            { Serial.println("Pressed Scan"); },
            LV_EVENT_PRESSED, nullptr);
        lv_group_add_obj(group, btn_scan);

        lv_obj_t *btn_attack = lv_list_add_btn(menu, LV_SYMBOL_CHARGE, "Attack");
        lv_obj_add_event_cb(
            btn_attack, [](lv_event_t *e)
            { Serial.println("Pressed Attack"); },
            LV_EVENT_PRESSED, nullptr);
        lv_group_add_obj(group, btn_attack);

        lv_obj_t *btn_info = lv_list_add_btn(menu, LV_SYMBOL_HOME, "Info");
        lv_obj_add_event_cb(
            btn_info, [](lv_event_t *e)
            { Serial.println("Pressed Info"); },
            LV_EVENT_PRESSED, nullptr);
        lv_group_add_obj(group, btn_info);

        lv_indev_set_group(ir_indev, group);

        lv_scr_load_anim(menu, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
    }
}
