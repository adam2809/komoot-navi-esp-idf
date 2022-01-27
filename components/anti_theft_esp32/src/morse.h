#ifndef _MORSE_H
#define _MORSE_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "button.h"
#include "alarm.h"

#define MAX_PASSWORD_LENGTH 10
#define MAX_BITS_IN_LETTER 5
#define TIME_BETWEEN_LETTERS 2000ULL
#define TIME_BETWEEN_WORDS 5000ULL
#define MAX_PASSWORD_TRIES 3

#define TAG "MORSE"

typedef struct{
    QueueHandle_t buttons_events;
    TaskHandle_t display_task_handle;
} morse_input_params_t;

char bin_morse_2_char(uint8_t bin_morse,uint8_t len);
void morse_password_input_task(void *pvParameter);
void read_morse_word(QueueHandle_t button_events,TaskHandle_t display_task_handle);
#endif