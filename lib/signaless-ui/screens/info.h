#pragma once

#include <lvgl.h>

#include "screens/common.h"

namespace UI {
    void load_screen_info() {
        lv_group_t *group = lv_group_create();
        lv_obj_t *scr = lv_obj_create(NULL);
        lv_obj_set_style_pad_all(scr, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_t *span_group = lv_spangroup_create(scr);
        lv_span_t *span;
        span = lv_spangroup_new_span(span_group);
        lv_spangroup_set_mode(span_group, LV_SPAN_MODE_BREAK);
        lv_spangroup_set_align(span_group, LV_TEXT_ALIGN_LEFT);
        lv_span_set_text(span, "ESP32-Signaless\n");
        lv_style_set_text_font(&span->style, &lv_font_montserrat_14);
        lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_RED));
        span = lv_spangroup_new_span(span_group);
        lv_span_set_text(span, "(c) 2022 BachNX.\n");
        lv_obj_set_size(span_group, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_align(span_group, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_group_add_obj(group, scr);
        lv_indev_set_group(ir_indev, group);
        auto handle_pressed = [](lv_event_t *e) {
            load_screen_menu();
        };
        lv_obj_add_event_cb(scr, handle_pressed, LV_EVENT_PRESSED, NULL);
        lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
    }
}