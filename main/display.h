#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "digit_imgs.h"
#include "freertos/FreeRTOS.h"

void init_lvgl_display(lv_color_t* buf);
void init_lvgl_objs();

#endif