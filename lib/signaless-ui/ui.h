#pragma once

#include <Arduino.h>

#ifndef IS_NATIVE
#include <TFT_eSPI.h>
#else
#include <SDL2/SDL.h>
#endif

#include <lvgl.h>
#include <vector>
#include "lv_indev_ir.h"
#include "badwifi.h"

#include "screens/common.h"
#include "screens/attack.h"
#include "screens/target_select.h"

namespace UI
{
    static const uint16_t screenWidth = TFT_WIDTH;
    static const uint16_t screenHeight = TFT_HEIGHT;
#ifndef IS_NATIVE
    TFT_eSPI tft = TFT_eSPI();
#else
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
#endif
    lv_indev_t *ir_indev;

    void tft_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
    {
#ifndef IS_NATIVE
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);
        tft.startWrite();
        tft.setAddrWindow(area->x1, area->y1, w, h);
        tft.pushColors((uint16_t *)&color_p->full, w * h, true);
        tft.endWrite();
#else
        SDL_Rect r;
        r.x = area->x1;
        r.y = area->y1;
        r.w = area->x2 - area->x1 + 1;
        r.h = area->y2 - area->y1 + 1;
        SDL_UpdateTexture(texture, &r, color_p, r.w * ((LV_COLOR_DEPTH + 7) / 8));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
#endif
        lv_disp_flush_ready(disp);
    }

    void setup()
    {
        static lv_disp_draw_buf_t draw_buf;
        static lv_color_t buf[screenWidth * screenHeight];
        lv_init();
#ifndef IS_NATIVE
        tft.begin();
        tft.setRotation(0);
#else
        window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, TFT_WIDTH, TFT_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_ALWAYS_ON_TOP);
        if (window == NULL) {
            printf("Unable to create SDL2 window");
            exit(1);
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, TFT_WIDTH, TFT_HEIGHT);
#endif
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
        // load_test_screen();
    }

    void tick()
    {
        static uint32_t last_draw = millis();
        uint32_t ellapsed = millis() - last_draw;
        last_draw = millis();
        lv_timer_handler();
        lv_tick_inc(ellapsed);
#ifdef IS_NATIVE
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
        }
#endif
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
            NULL);
        lv_timer_set_repeat_count(timer, 1);
    }

    void handle_click_attack(lv_event_t *e)
    {
        std::vector<BadWifi::AP> targets = BadWifi::get_targets();
        if (targets.empty()) {
            msgbox_show("Error", "No target chosen!");
        } else {
            // TODO:
        }
    }

    void load_screen_menu()
    {
        lv_group_t *group = lv_group_create();
        lv_obj_t *scr = lv_obj_create(NULL);
        lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(scr, 0, 0);

        lv_obj_t *title = lv_label_create(scr);
        lv_label_set_text(title, "ESP32-Signaless");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_hor(title, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_ver(title, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *menu = lv_list_create(scr);
        lv_obj_set_size(menu, 112, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_row(menu, 0, 0);

        lv_obj_t *btn_attack = lv_list_add_btn(menu, LV_SYMBOL_CHARGE, "Attack");
        lv_obj_set_style_text_color(btn_attack, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_hor(btn_attack, 0, 0);
        lv_obj_add_event_cb(btn_attack, handle_click_attack, LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn_attack);

        lv_obj_t *btn_targets = lv_list_add_btn(menu, LV_SYMBOL_LIST, "Targets");
        lv_obj_set_style_pad_hor(btn_targets, 0, 0);
        lv_obj_add_event_cb(
            btn_targets, [](lv_event_t *e)
            { load_screen_target_select(); },
            LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn_targets);

        lv_obj_t *btn_scan = lv_list_add_btn(menu, LV_SYMBOL_WIFI, "Scan");
        lv_obj_set_style_pad_hor(btn_scan, 0, 0);
        lv_obj_add_event_cb(
            btn_scan, [](lv_event_t *e)
            { Serial.println("Pressed Scan"); },
            LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn_scan);

        lv_obj_t *btn_info = lv_list_add_btn(menu, LV_SYMBOL_HOME, "Info");
        lv_obj_set_style_pad_hor(btn_info, 0, 0);
        lv_obj_add_event_cb(
            btn_info, [](lv_event_t *e)
            { Serial.println("Pressed Info"); },
            LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn_info);

        lv_indev_set_group(ir_indev, group);
        lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, true);
    }

    void load_test_screen() {
        lv_obj_t *scr = lv_obj_create(NULL);
        lv_obj_t *list = lv_list_create(scr);
        lv_obj_set_size(list, lv_pct(100), lv_pct(100));
        lv_obj_center(list);
        lv_group_t *group = lv_group_create();

        for (int i = 0; i < 30; i++) {
            lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "Hello world");
            lv_group_add_obj(group, btn);
        }

        lv_scr_load(scr);
        lv_indev_set_group(ir_indev, group);
    }
}
