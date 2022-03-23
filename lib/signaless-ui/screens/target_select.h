#pragma once

#ifndef IS_NATIVE
#include <Arduino.h>
#endif

#include <lvgl.h>
#include <vector>
#include "common.h"
#include "badwifi.h"

namespace UI
{
    void load_screen_target_select() {
        lv_obj_t *scr = lv_obj_create(NULL);
        lv_obj_t *list = lv_list_create(scr);
        lv_obj_set_size(list, lv_pct(100), lv_pct(100));
        lv_obj_center(list);

        static std::vector<BadWifi::AP> items;
        items = BadWifi::get_targets();
        static lv_group_t *group;
        group = lv_group_create();

        auto handle_remove_item = [] (lv_event_t *e) {
            const char *ssid = (const char *) e->user_data;
            for (auto it = items.begin(); it != items.end(); it++) {
                if (strcmp(it->ssid.c_str(), ssid) == 0) {
                    items.erase(it);
                    break;
                }
            }
            lv_obj_t *btn = e->target;
            lv_obj_del(btn);
            lv_group_focus_next(group);
        };

        auto handle_remove_all = [] (lv_event_t *e) {
            items.clear();
            BadWifi::set_targets(items);
            load_screen_menu();
            msgbox_show("Info", "Targets removed!");
        };

        auto handle_save = [] (lv_event_t *e) {
            BadWifi::set_targets(items);
            load_screen_menu();
            msgbox_show("Info", "Targets saved!");
        };

        auto handle_cancel = [] (lv_event_t *e) {
            load_screen_menu();
        };

        lv_obj_t *btn;

        for (const auto &it : items) {
            btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, it.ssid.c_str());
            lv_obj_add_event_cb(btn, handle_remove_item, LV_EVENT_PRESSED, (void *)it.ssid.c_str());
            lv_group_add_obj(group, btn);
        }

        btn = lv_list_add_btn(list, LV_SYMBOL_EJECT, "Remove all");
        lv_obj_add_event_cb(btn, handle_remove_all, LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn);

        btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, "Save");
        lv_obj_add_event_cb(btn, handle_save, LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn);

        btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Cancel");
        lv_obj_add_event_cb(btn, handle_cancel, LV_EVENT_PRESSED, NULL);
        lv_group_add_obj(group, btn);

        lv_indev_set_group(ir_indev, group);

        lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
    }
}
