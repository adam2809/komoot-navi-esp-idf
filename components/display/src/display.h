#pragma once

#include "images/digits.h"
#include "images/misc.h"
#include "images/dir_symbols.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "i2c_manager.h"
#include "morse.h"

#define LV_TICK_PERIOD_MS 1

enum display_task_notify_value_t{
    NOTIFY_VALUE_NAVIGATION,
    NOTIFY_VALUE_PASSKEY,
    NOTIFY_VALUE_MORSE
};

void display_task(void *pvParameter);
static void lv_tick_task(void *arg);

void init_lvgl_display(lv_color_t* buf);
void init_lvgl_objs();
void display_passkey(uint32_t passkey);
void display_dir_symbol(uint8_t symbol);
void display_meters(uint32_t meters);
void display_morse(uint8_t bin_morse,uint8_t len,char* password);

