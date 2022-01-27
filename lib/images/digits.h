#pragma once

#define DIGITS_IN_ROW_COUNT 3

#define DIGIT_1_Y_OFFSET -2
#define DIGIT_Y_SPACING 20

#define DIGITS_ROW_TOP_X_OFFSET 17
#define DIGITS_ROW_BOTTOM_X_OFFSET 79

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
#ifndef LV_ATTRIBUTE_IMG_ZERO
#define LV_ATTRIBUTE_IMG_ZERO
#endif


#include <stdio.h>
#include "lvgl_helpers.h"

const lv_img_dsc_t* digits[10];
