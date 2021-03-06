#pragma once

#include "images/digits.h"
#include "images/misc.h"
#include "images/dir_symbols.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "i2c_manager.h"
#include "morse.h"
#include "komoot_ble_client.h"

#define LV_TICK_PERIOD_MS 1

enum display_task_notify_value_t{
    NOTIFY_VALUE_CLEAR,
    NOTIFY_VALUE_NAVIGATION,
    NOTIFY_VALUE_BT_CONNECT,
    NOTIFY_VALUE_BT_DISCONNECT,
    NOTIFY_VALUE_PASSKEY,
    NOTIFY_VALUE_MORSE,
    NOTIFY_VALUE_WRONG_PASS,
    NOTIFY_VALUE_LOCK,
    NOTIFY_VALUE_UNLOCK,
    NOTIFY_VALUE_ALARM
};

void display_task(void *pvParameter);
static void lv_tick_task(void *arg);

void init_lvgl_display(lv_color_t* buf);
