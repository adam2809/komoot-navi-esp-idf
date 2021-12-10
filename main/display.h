#ifndef _DISPLAY_H
#define _DISPLAY_H

#define DISPLAY_TAG "DISPLAY"

#include "digits.h"
#include "misc.h"
#include "dir_symbols.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "i2c_manager.h"

void init_lvgl_display(lv_color_t* buf);
void init_lvgl_objs();
void display_passkey(uint32_t passkey);
void display_dir_symbol(uint8_t symbol);
void display_meters(uint32_t meters);
void display_morse(uint8_t bin_morse,uint8_t len,char* password,uint8_t password_len);

#endif