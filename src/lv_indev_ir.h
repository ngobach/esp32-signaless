#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <IRremote.h>

namespace LvIndevIr
{
    const int IR_PIN = 16;

    lv_indev_drv_t _ir_remote_drv;
    uint32_t last_key = -1;
    uint32_t last_read = 0;

    void lv_indev_ir_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
    {
        const uint16_t KEY_LEFT = 8;
        const uint16_t KEY_OK = 28;
        const uint16_t KEY_RIGHT = 90;
        const uint16_t KEY_UP = 24;
        const uint16_t KEY_DOWN = 82;

        if (IrReceiver.decode())
        {
            uint16_t command = IrReceiver.decodedIRData.command;

            // Serial.println(results.value);
            uint32_t matched_key = 0;
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
            if (matched_key) {
                data->key = matched_key;
                data->state = LV_INDEV_STATE_PRESSED;
                last_key = matched_key;
                last_read = millis();
            }
            IrReceiver.resume();
        }
        else
        {
            if (millis() - last_read < 100) {
                data->state = LV_INDEV_STATE_PRESSED;
                data->key = last_key;
            } else {
                data->state = LV_INDEV_STATE_RELEASED;
                last_key = 0;
                last_read = 0;
            }
        }
    }

    lv_indev_drv_t *lv_indev_drv_ir_create()
    {
        lv_indev_drv_init(&_ir_remote_drv);
        _ir_remote_drv.type = LV_INDEV_TYPE_ENCODER;
        _ir_remote_drv.read_cb = &lv_indev_ir_read;
        IrReceiver.begin(IR_PIN);
        IrReceiver.enableIRIn();

        return &_ir_remote_drv;
    }
}

lv_indev_drv_t *lv_indev_drv_ir_create()
{
    return LvIndevIr::lv_indev_drv_ir_create();
}