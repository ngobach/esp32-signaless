#pragma once

#include <Arduino.h>
#ifndef IS_NATIVE
#include <IRremote.h>
#endif

#include <lvgl.h>

namespace LvIndevIr
{
    const int IR_PIN = 16;
    const bool FIRE_ONCE = false;

    lv_indev_drv_t _ir_remote_drv;
    uint32_t last_key = -1;
    uint32_t last_read = 0;
    bool fired = false;

    void lv_indev_ir_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
    {
#ifndef IS_NATIVE
        const uint16_t KEY_LEFT = 8;
        const uint16_t KEY_OK = 28;
        const uint16_t KEY_RIGHT = 90;
        const uint16_t KEY_UP = 24;
        const uint16_t KEY_DOWN = 82;

        uint32_t matched_key = 0;

        if (IrReceiver.decode())
        {
            uint16_t command = IrReceiver.decodedIRData.command;

            // Serial.println(results.value);
            if (command == KEY_LEFT)
            {
                matched_key = LV_KEY_LEFT;
            }
            else if (command == KEY_DOWN)
            {
                matched_key = LV_KEY_DOWN;
            }
            else if (command == KEY_RIGHT)
            {
                matched_key = LV_KEY_RIGHT;
            }
            else if (command == KEY_UP)
            {
                matched_key = LV_KEY_UP;
            }
            else if (command == KEY_OK)
            {
                matched_key = LV_KEY_ENTER;
            }
            else
            {
                // Serial.println("Failed");
                // Serial.println(command);
            }
            last_read = millis();
            IrReceiver.resume();
        }
        else
        {
            if (millis() - last_read < 100) {
                matched_key = last_key;
            }
        }

        if (matched_key) {
            if (!FIRE_ONCE || !fired) {
                data->key = matched_key;
                data->state = LV_INDEV_STATE_PRESSED;
                last_key = matched_key;
                fired = true;
            } else {
                data->state = LV_INDEV_STATE_RELEASED;
            }
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
            last_key = 0;
            last_read = 0;
            fired = false;
        }
#else
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT]) {
        data->key = LV_KEY_LEFT;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (keys[SDL_SCANCODE_RIGHT]) {
        data->key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (keys[SDL_SCANCODE_RETURN]) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
#endif
    }

    lv_indev_drv_t *lv_indev_drv_ir_create()
    {
        lv_indev_drv_init(&_ir_remote_drv);
        _ir_remote_drv.type = LV_INDEV_TYPE_ENCODER;
        _ir_remote_drv.read_cb = &lv_indev_ir_read;
#ifndef IS_NATIVE
        IrReceiver.begin(IR_PIN);
        IrReceiver.enableIRIn();
#endif
        return &_ir_remote_drv;
    }
}

lv_indev_drv_t *lv_indev_drv_ir_create()
{
    return LvIndevIr::lv_indev_drv_ir_create();
}