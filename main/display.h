#ifndef _DISPLAY_H
#define _DISPLAY_H

#define DISPLAY_TAG "DISPLAY"

#include "digit_imgs.h"
#include "misc_imgs.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

void init_lvgl_display(lv_color_t* buf);
void init_lvgl_objs();
void display_passkey(uint32_t passkey);
void display_nav_symbol(uint8_t symbol);
void display_meters(uint32_t meters);

#endif