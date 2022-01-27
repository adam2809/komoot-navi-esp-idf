#pragma once

#include "mpu6050.h"
#include "button.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_sleep.h"

void alarm_enable_task(void *pvParameter);
void go_to_deep_sleep();