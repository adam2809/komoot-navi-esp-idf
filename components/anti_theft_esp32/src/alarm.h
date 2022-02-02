#pragma once

#include "mpu6050.h"
#include "button.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "morse.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define ALARM_NOTIF_FLASHING_FREQ 200

struct morse_input_params_s;

void alarm_init(struct morse_input_params_s params);

bool get_alarm_state();
void raise_alarm_state();
void lower_alarm_state();

bool get_lock_state();
void init_lock_state();
void lock();
void unlock();

void alarm_button_disable_task(void *pvParameter);
void go_to_deep_sleep(bool locked);